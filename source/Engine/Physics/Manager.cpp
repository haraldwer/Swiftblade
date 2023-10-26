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
    Material = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    PxRigidStatic* groundPlane = PxCreatePlane(*Physics, PxPlane(0,1,0,0), *Material);
    Scene->addActor(*groundPlane);

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
    Scene->simulate(static_cast<PxReal>(InDelta));
    Scene->fetchResults(true);

    for (auto& instance : Instances)
    {
        CHECK_CONTINUE(!instance.second)

        
    }
    
    // Apply result 
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
        Utility::PhysX::ConvertMat(Mat4F(
            QuatF::Identity(),
            t->Position,
            Vec3F::One())));
    
    bool stationary = true; 
    PxRigidActor* actor = nullptr;
    
    if (rb)
    {
        PxRigidDynamic* ptr = PxCreateDynamic(*Physics, trans, PxBoxGeometry(1, 1, 1), *Material, 1.0f);
        actor = ptr;
    }
    else if (!stationary)
    {
        PxRigidDynamic* ptr = PxCreateDynamic(*Physics, trans, PxBoxGeometry(1, 1, 1), *Material, 1.0f);
        Rigidbodies[InID] = ptr; 
        actor = ptr;
    }
    else
    {
        
        PxRigidStatic* ptr = PxCreateStatic(*Physics, trans, PxBoxGeometry(1, 1, 1), *Material);
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
    if (Rigidbodies.contains(InID))
        Rigidbodies.erase(InID);
}
