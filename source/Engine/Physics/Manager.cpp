#include "Manager.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"

#include "Utility/PhysXUtility.h"

#define PVD_HOST "127.0.0.1"

static physx::PxDefaultAllocator gAllocator;
static physx::PxDefaultErrorCallback gErrorCallback;

using namespace physx;

void Physics::Manager::Init()
{
    Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    PVD = PxCreatePvd(*Foundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    PVD->connect(*transport,PxPvdInstrumentationFlag::eALL);

    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(),true,PVD);

    PxSceneDesc sceneDesc(Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    Dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= Dispatcher;
    sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
    Scene = Physics->createScene(sceneDesc);

    if(PxPvdSceneClient* pvdClient = Scene->getScenePvdClient())
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    if (const PhysicsMaterialResource* matRsc = ResPhysicsMaterial("physMat_default.json").Get())
    {
        PxRigidStatic* groundPlane = PxCreatePlane(*Physics, PxPlane(0,1,0,0), *matRsc->Get());
        Scene->addActor(*groundPlane);
    }

}

void Physics::Manager::Deinit()
{
    PX_RELEASE(Scene);
    PX_RELEASE(Dispatcher);
    PX_RELEASE(Physics);
    if(PVD)
    {
        PxPvdTransport* transport = PVD->getTransport();
        PVD->release();
        PVD = nullptr;
        PX_RELEASE(transport);
    }
    PX_RELEASE(Foundation);
}

void Physics::Manager::Update(double InDelta)
{
    const auto& ecs = ECS::Manager::Get();
    for (const auto& instance : Dynamics)
    {
        CHECK_CONTINUE(!instance.second)
        auto t = ecs.GetComponent<ECS::Transform>(instance.first);
        CHECK_CONTINUE(!t);
        const PxTransform trans = PxTransform(
            Utility::PhysX::ConvertVec(t->Position),
            Utility::PhysX::ConvertQuat(t->Rotation));
        instance.second->setGlobalPose(trans);
    }

    Scene->simulate(static_cast<PxReal>(InDelta));
    Scene->fetchResults(true);

    for (const auto& instance : Dynamics)
    {
        CHECK_CONTINUE(!instance.second)
        auto t = ecs.GetComponent<ECS::Transform>(instance.first);
        CHECK_CONTINUE(!t);
        PxTransform trans = instance.second->getGlobalPose();
        t->Position = Utility::PhysX::ConvertVec(trans.p);
        t->Rotation = Utility::PhysX::ConvertQuat(trans.q);
    }
}

void Physics::Manager::Add(const ECS::EntityID InID)
{
    const auto& ecs = ECS::Manager::Get();
    const auto t = ecs.GetComponent<ECS::Transform>(InID);
    const auto c = ecs.GetComponent<ECS::Collider>(InID); 
    const auto rb = ecs.GetComponent<ECS::Rigidbody>(InID);

    CHECK_RETURN_LOG(!t, "No transform for entity");
    CHECK_RETURN_LOG(!c, "No collider for entity");

    const PxTransform trans = PxTransform(
        Utility::PhysX::ConvertVec(t->Position),
        Utility::PhysX::ConvertQuat(t->Rotation));

    PxMaterial* material = nullptr;
    if (PhysicsMaterialResource* matRsc = c->Material.Get().Get())
        material = matRsc->Get();
    CHECK_RETURN(!material); 
    
    PxRigidActor* actor = nullptr;
    
    if (!t->Static)
    {
        PxRigidDynamic* ptr = PxCreateDynamic(
            *Physics,
            trans,
            PxBoxGeometry(1, 1, 1),
            *material,
            1.0f);
        Dynamics[InID] = ptr; 
        actor = ptr;
        PxRigidDynamicLockFlags flags;
        
        if (!rb)
        {
            flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
            flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
            flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
            flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
            flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
            flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
        }
        else
        {
            if (rb->LockRotation)
            {
                flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
                flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
                flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
            }
        }

        ptr->setRigidDynamicLockFlags(flags);
    }
    else
    {
        PxRigidStatic* ptr = PxCreateStatic(
            *Physics,
            trans,
            PxBoxGeometry(1, 1, 1),
            *material);
        actor = ptr;
    }

    Instances[InID] = actor;
    Scene->addActor(*actor);
}

void Physics::Manager::Remove(const ECS::EntityID InID)
{
    if (const auto ptr = Instances[InID])
    {
        Scene->removeActor(*Instances[InID]);
        ptr->release();
    }
    Instances.erase(InID);
    if (Dynamics.contains(InID))
        Dynamics.erase(InID);
}

void Physics::Manager::AddForce(ECS::EntityID InID, const Vec3F& InForce, ForceMode InForceMode)
{
    const auto find = Dynamics.find(InID);
    CHECK_RETURN(find == Dynamics.end())
    CHECK_RETURN(!find->second);
    const PxVec3 force = Utility::PhysX::ConvertVec(InForce);
    const PxForceMode::Enum mode = ConvertForceMode(InForceMode);
    find->second->addForce(force, mode);
}

PxMaterial* Physics::Manager::CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution) const
{
    CHECK_RETURN(!Physics, nullptr); 
    return Physics->createMaterial(InStaticFric, InDynamicFric, InRestitution);
}

PxForceMode::Enum Physics::Manager::ConvertForceMode(ForceMode InMode)
{
    switch (InMode)
    {
    case ForceMode::FORCE:
        return PxForceMode::eFORCE;
    case ForceMode::IMPULSE:
        return PxForceMode::eIMPULSE;
    case ForceMode::VELOCITY:
        return PxForceMode::eVELOCITY_CHANGE;
    }
    return PxForceMode::eFORCE;
}
