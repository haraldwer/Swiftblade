#include "Manager.h"

#include "Engine/ECS/Manager.h"

#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"

// Main physx includes
#include "Utility/PhysXUtility.h"
#include "physx/PxPhysicsAPI.h"

#include "Callback.h"

#define PVD_HOST "127.0.0.1"

static physx::PxDefaultAllocator gAllocator;
static physx::PxDefaultErrorCallback gErrorCallback;

static Physics::PersistentPhysics Persistent;

using namespace physx;

void Physics::PersistentPhysics::TryInit()
{
    if (Foundation && Physics && Dispatcher)
        return; 
    
    Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    if (!PVD)
    {
        PVD = PxCreatePvd(*Foundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        PVD->connect(*transport,PxPvdInstrumentationFlag::eALL);
    }

    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(),true,PVD);
    Dispatcher = PxDefaultCpuDispatcherCreate(2); 
}

Physics::PersistentPhysics::~PersistentPhysics()
{
    PX_RELEASE(Dispatcher);
    PX_RELEASE(Physics);
    if(PVD)
    {
        if (PVD->isConnected())
            PVD->disconnect();
        PxPvdTransport* transport = PVD->getTransport();
        PX_RELEASE(transport);
        PX_RELEASE(PVD);
    }
    PX_RELEASE(Foundation);
}

void Physics::Manager::Init()
{
    Persistent.TryInit();

    if (!Callback)
        Callback = new Physics::Callback();
    
    PxSceneDesc sceneDesc(Persistent.Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f); // Apply gravity manually
    sceneDesc.cpuDispatcher	= Persistent.Dispatcher;
    sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
    sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
    sceneDesc.filterShader = Callback::contactReportFilterShader;
    sceneDesc.simulationEventCallback = Callback; 
    
    Scene = Persistent.Physics->createScene(sceneDesc);

    if(PxPvdSceneClient* pvdClient = Scene->getScenePvdClient())
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    if (const PhysicsMaterialResource* matRsc = ResPM("Defaults/PM_Default.json").Get())
    {
        PxRigidStatic* groundPlane = PxCreatePlane(*Persistent.Physics, PxPlane(0,1,0,0), *matRsc->Get());
        Scene->addActor(*groundPlane);
    }
}

void Physics::Manager::Deinit()
{
    if (Callback)
    {
        delete(Callback);
        Callback = nullptr;
    }
    PX_RELEASE(Scene);
}

void Physics::Manager::Update(double InDelta) const
{
    const auto& ecs = ECS::Manager::Get();
    const auto updatePhysTrans = [](const ECS::Manager& InECS, ECS::EntityID InKey, auto* InInstance)
    {
        CHECK_RETURN(!InInstance)
        auto* t = InECS.GetComponent<ECS::Transform>(InKey);
        CHECK_RETURN(!t);
        const PxTransform trans = PxTransform(
            Utility::PhysX::ConvertVec(t->GetPosition()),
            Utility::PhysX::ConvertQuat(t->GetRotation()));
        if (InInstance->getGlobalPose() != trans)
            InInstance->setGlobalPose(trans);
    };
    
    for (const auto& instance : Statics)
        updatePhysTrans(ecs, instance.first, instance.second);
    for (const auto& instance : Dynamics)
        updatePhysTrans(ecs, instance.first, instance.second);

    Scene->simulate(static_cast<PxReal>(InDelta));
    Scene->fetchResults(true);

    for (const auto& instance : Dynamics)
    {
        CHECK_CONTINUE(!instance.second)
        auto* t = ecs.GetComponent<ECS::Transform>(instance.first);
        CHECK_CONTINUE(!t);
        PxTransform trans = instance.second->getGlobalPose();
        const Vec3F p = Utility::PhysX::ConvertVec(trans.p);
        const QuatF q = Utility::PhysX::ConvertQuat(trans.q);
        const Mat4F mat = Mat4F(p, q, t->GetScale());
        t->SetWorld(mat); 
    }
}

void Physics::Manager::Add(const ECS::EntityID InID)
{
    const auto& ecs = ECS::Manager::Get();
    const auto collider = ecs.GetComponent<ECS::Collider>(InID); 
    CHECK_RETURN_LOG(!collider, "No collider for entity");
    
    ECS::EntityID actorID = ECS::InvalidID;
    PxRigidActor* actor = nullptr;
    
    if (const ECS::Rigidbody* rb = FindRigidbody(InID)) // Find rigidbody in hierarchy
    {
        const ECS::EntityID rbID = rb->GetID(); 
        PxRigidDynamic*& ptr = Dynamics[rbID];
        if (!ptr) // dynamic has not yet been added, create new!
            ptr = CreateDynamic(*rb);
        actor = ptr;
        actorID = rbID; 
    }
    else // No rb, static object
    {
        actor = CreateStatic(*collider);
        actorID = InID; 
    }

    CHECK_RETURN_LOG(actorID == ECS::InvalidID, "ActorID invalid");
    CHECK_RETURN_LOG(!actor, "Actor invalid");
    CreateShape(*collider, actorID, *actor);     
}

ECS::Rigidbody* Physics::Manager::FindRigidbody(const ECS::EntityID InID)
{
    const auto& ecs = ECS::Manager::Get();
    ECS::EntityID currID = InID;
    while (currID != ECS::InvalidID)
    {
        if (const auto currRB = ecs.GetComponent<ECS::Rigidbody>(currID))
            return currRB;
        if (const auto currTrans = ecs.GetComponent<ECS::Transform>(currID))
            currID = currTrans->GetParent();
    }
    return nullptr; 
}

PxRigidDynamic* Physics::Manager::CreateDynamic(const ECS::Rigidbody& InRigidbody)
{
    const auto& ecs = ECS::Manager::Get();
    const auto trans = ecs.GetComponent<ECS::Transform>(InRigidbody.GetID());
    CHECK_RETURN_LOG(!trans, "No transform", nullptr);

    const PxTransform pose = PxTransform(
            Utility::PhysX::ConvertVec(trans->GetPosition()),
            Utility::PhysX::ConvertQuat(trans->GetRotation()));
    
    PxRigidDynamic* ptr = Persistent.Physics->createRigidDynamic(pose);
    CHECK_RETURN_LOG(!ptr, "Failed to create dynamic", nullptr);

    if (InRigidbody.LockRotation)
    {
        PxRigidDynamicLockFlags flags;
        flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
        flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
        flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
        ptr->setRigidDynamicLockFlags(flags);
    }

    ptr->setMass(InRigidbody.Mass);
    ptr->setLinearDamping(InRigidbody.LinearDamping);
    ptr->setAngularDamping(InRigidbody.AngularDamping);
    ptr->setMaxLinearVelocity(InRigidbody.MaxLinearVelocity);
    ptr->setMaxAngularVelocity(InRigidbody.MaxAngularVelocity);
    
    Dynamics[InRigidbody.GetID()] = ptr;
    Scene->addActor(*ptr);
    return ptr;
}

PxRigidStatic* Physics::Manager::CreateStatic(const ECS::Collider& InCollider)
{
    const auto& ecs = ECS::Manager::Get();
    const auto trans = ecs.GetComponent<ECS::Transform>(InCollider.GetID());
    CHECK_RETURN_LOG(!trans, "No transform", nullptr); 

    const PxTransform pose = PxTransform(
            Utility::PhysX::ConvertVec(trans->GetPosition()),
            Utility::PhysX::ConvertQuat(trans->GetRotation()));
        
    PxRigidStatic* ptr = Persistent.Physics->createRigidStatic(pose);
    CHECK_RETURN_LOG(!ptr, "Failed to create static", nullptr);
    Statics[InCollider.GetID()] = ptr;
    Scene->addActor(*ptr);
    return ptr; 
}

void Physics::Manager::CreateShape(const ECS::Collider& InCollider, const ECS::EntityID InActorID, PxRigidActor& InActor)
{
    const ECS::EntityID id = InCollider.GetID();
    const auto& ecs = ECS::Manager::Get();
    const auto trans = ecs.GetComponent<ECS::Transform>(id);
    CHECK_RETURN_LOG(!trans, "No transform");
    
    // Get material
    const PxMaterial* material = nullptr;
    if (const PhysicsMaterialResource* matRsc = InCollider.Material.Get().Get())
        material = matRsc->Get();
    CHECK_RETURN_LOG(!material, "Failed to get material");

    // Get geometry
    const PxGeometry* geometry = GetGeometry(
        static_cast<ECS::CollisionShape>(InCollider.Shape.Get()),
        InCollider.ShapeData.Get());
    
    // Create shape
    PxShape* shape = Persistent.Physics->createShape(
        *geometry,
        *material,
        true);
    InActor.attachShape(*shape);

    // Set transform
    if (InActorID != id)
    {
        const PxTransform localTrans = PxTransform(
        Utility::PhysX::ConvertVec(trans->GetPosition(ECS::Transform::Space::LOCAL)),
        Utility::PhysX::ConvertQuat(trans->GetRotation(ECS::Transform::Space::LOCAL)));
        shape->setLocalPose(localTrans); // TODO: Has to be local relative to actor
    }

    // Set shape user data (very unsafe)
    shape->userData = ECS::EntityToPtr(InActorID); 
        
    // Add to maps
    Shapes[id] = shape;
    ShapeToActor[id] = InActorID;
    ActorToShape[InActorID].insert(id);
}

void Physics::Manager::Remove(const ECS::EntityID InID)
{
    TryReleaseShape(InID); 
    TryReleaseStatic(InID); 
}

void Physics::Manager::TryReleaseShape(ECS::EntityID InID)
{
    const auto shapeFind = Shapes.find(InID);
    CHECK_RETURN(shapeFind == Shapes.end());
    
    // Release shape 
    PxShape* shape = shapeFind->second;
    if (PxRigidActor* actor = shape->getActor())
        actor->detachShape(*shape);
    shape->release();
    Shapes.erase(InID);

    // Remove attachment data
    const auto rbFind = ShapeToActor.find(InID); 
    CHECK_RETURN(rbFind == ShapeToActor.end());
    const ECS::EntityID rbID = rbFind->second;

    // Remove from shape -> actor map
    ShapeToActor.erase(InID);

    // Remove from actor -> shape set 
    const auto rbAttachments = ActorToShape.find(rbID);
    CHECK_RETURN(rbAttachments == ActorToShape.end());
    CHECK_RETURN(!rbAttachments->second.contains(InID))
    rbAttachments->second.erase(InID); 
    if (rbAttachments->second.empty())
    {
        // No shapes attached, release actor
        TryReleaseDynamic(rbID); 
        TryReleaseStatic(rbID); 
    }
}

void Physics::Manager::TryReleaseDynamic(ECS::EntityID InID)
{
    CHECK_RETURN(!Dynamics.contains(InID));
    Dynamics[InID]->release();
    Dynamics.erase(InID);
}

void Physics::Manager::TryReleaseStatic(ECS::EntityID InID)
{
    CHECK_RETURN(!Statics.contains(InID)); 
    Statics[InID]->release();
    Statics.erase(InID); 
}

PxMaterial* Physics::Manager::CreateMaterial(const float InStaticFric, const float InDynamicFric, const float InRestitution)
{
    return Persistent.Physics->createMaterial(InStaticFric, InDynamicFric, InRestitution);
}

physx::PxGeometry* Physics::Manager::GetGeometry(ECS::CollisionShape InShape, const Vec4F& InShapeData)
{
    switch (InShape)
    {
    case ECS::CollisionShape::BOX:
        {
            static PxBoxGeometry box;
            box = PxBoxGeometry(InShapeData.x, InShapeData.y, InShapeData.z);
            return &box; 
        }
    case ECS::CollisionShape::CAPSULE:
        {
            static PxCapsuleGeometry capsule;
            capsule = PxCapsuleGeometry(InShapeData.x, InShapeData.y);
            return &capsule;
        }
    case ECS::CollisionShape::SPHERE:
        {
            static PxSphereGeometry sphere;
            sphere = PxSphereGeometry(InShapeData.x);
            return &sphere;
        }
        // Consider cooking geometry
    }
    return nullptr;
}

void Physics::Manager::AddForce(ECS::EntityID InID, const Vec3F& InForce, ForceMode InForceMode)
{
    const auto find = Dynamics.find(InID);
    CHECK_RETURN(find == Dynamics.end())
    CHECK_RETURN(!find->second);
    const PxVec3 force = Utility::PhysX::ConvertVec(InForce);

    auto convertForceMode = [](const ForceMode InMode)
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
    };
    
    const PxForceMode::Enum mode = convertForceMode(InForceMode);
    find->second->addForce(force, mode);
}

void Physics::Manager::SetVelocity(const ECS::EntityID InID, const Vec3F& InVelocity)
{
    const auto find = Dynamics.find(InID);
    CHECK_RETURN(find == Dynamics.end())
    CHECK_RETURN(!find->second);
    find->second->setLinearVelocity(
        Utility::PhysX::ConvertVec(InVelocity));
}

Vec3F Physics::Manager::GetVelocity(const ECS::EntityID InID)
{
    const auto find = Dynamics.find(InID);
    CHECK_RETURN(find == Dynamics.end(), Vec3F())
    CHECK_RETURN(!find->second, Vec3F());
    const PxVec3 vel = find->second->getLinearVelocity();
    return Utility::PhysX::ConvertVec(vel); 
}

void Physics::Manager::ClearForces(ECS::EntityID InID)
{
    const auto find = Dynamics.find(InID);
    CHECK_RETURN(find == Dynamics.end())
    CHECK_RETURN(!find->second);
    find->second->clearForce();
}

void Physics::Manager::SetKinematic(ECS::EntityID InID, bool InKinematic)
{
    const auto find = Dynamics.find(InID);
    CHECK_RETURN(find == Dynamics.end())
    CHECK_RETURN(!find->second);
    auto flags = find->second->getRigidBodyFlags();
    InKinematic ?
        flags |= PxRigidBodyFlag::eKINEMATIC : // Set this bit
        flags &= ~PxRigidBodyFlag::eKINEMATIC; // Keep all but this bit
    find->second->setRigidBodyFlags(flags); 
}

Physics::TraceResult Physics::Manager::Trace(const Vec3F& aStart, const Vec3F& anEnd) const
{
    Vec3F diff = (anEnd - aStart);
    Vec3F dir = diff.GetNormalized();
    float length = diff.Length();
    
    TraceResult result;

    constexpr size_t numHits = 8;
    PxRaycastHit hits[numHits];
    PxRaycastBuffer buff(hits, numHits);

    // TODO: Filtering 
    Scene->raycast(
        Utility::PhysX::ConvertVec(aStart),
        Utility::PhysX::ConvertVec(dir),
        length,
        buff);

    if (buff.nbTouches > 0)
    {
        result.IsHit = true;
        for (int i = 0; i < static_cast<int>(buff.nbTouches); i++)
        {
            auto& touch = buff.touches[i];

            if (!(static_cast<uint8>(touch.shape->getFlags()) &
                static_cast<uint8>(PxShapeFlag::eSCENE_QUERY_SHAPE)))
                continue;
            
            auto& hit = result.Hits.emplace_back(); 
            hit.Distance = touch.distance;
            hit.Position = Utility::PhysX::ConvertVec(touch.position);
            hit.Normal = Utility::PhysX::ConvertVec(touch.normal);
            hit.Entity = ECS::PtrToEntity(touch.shape->userData);
            result.Hits.push_back(hit);
        }
    }

    return result;
}

Physics::TraceResult Physics::Manager::Sweep(const Vec3F& aStart, const Vec3F& anEnd, ECS::CollisionShape InShape, const Vec4F& InShapeData, const Mat4F& InPose) const
{
    Vec3F diff = (anEnd - aStart);
    Vec3F dir = diff.GetNormalized();
    float length = diff.Length();
    
    TraceResult result;

    PxGeometry* geometry = GetGeometry(InShape, InShapeData);
    CHECK_RETURN(!geometry, result);
    
    constexpr size_t numHits = 8;
    PxSweepHit hits[numHits];
    PxSweepBuffer buff(hits, numHits);

    const PxTransform pose = PxTransform(
        Utility::PhysX::ConvertVec(aStart + InPose.GetPosition()),
        Utility::PhysX::ConvertQuat(InPose.GetRotation()));
    
    // TODO: Filtering
    Scene->sweep(
        *geometry,
        pose,
        Utility::PhysX::ConvertVec(dir),
        length,
        buff);
    
    if (buff.nbTouches > 0)
    {
        result.IsHit = true;
        for (int i = 0; i < static_cast<int>(buff.nbTouches); i++)
        {
            auto& touch = buff.touches[i];

            if (!(static_cast<uint8>(touch.shape->getFlags()) &
                static_cast<uint8>(PxShapeFlag::eSCENE_QUERY_SHAPE)))
                    continue;
            
            auto& hit = result.Hits.emplace_back(); 
            hit.Distance = touch.distance;
            hit.Position = Utility::PhysX::ConvertVec(touch.position);
            hit.Normal = Utility::PhysX::ConvertVec(touch.normal);
            hit.Entity = ECS::PtrToEntity(touch.shape->userData);
            result.Hits.push_back(hit);
        }
    }

    return result;
}

void Physics::Manager::AddCubes(ECS::EntityID InID, const Vector<Vec3F>& InPositions)
{
    // Create cube owner
    if (!CubeOwner)
    {
        const PxTransform pose = PxTransform(
            Utility::PhysX::ConvertVec(Vec3F::Zero()),
            Utility::PhysX::ConvertQuat(QuatF::Identity()));
        CubeOwner = Persistent.Physics->createRigidStatic(pose);
        CHECK_RETURN_LOG(!CubeOwner, "Failed to create cube actor");
        Scene->addActor(*CubeOwner);
    }
    
    // Get material
    const PxMaterial* material = nullptr;
    if (const PhysicsMaterialResource* matRsc = ResPM("Defaults/PM_Default.json").Get())
        material = matRsc->Get();
    CHECK_RETURN_LOG(!material, "Failed to get material");

    // Get geometry
    const PxGeometry* geometry = GetGeometry(
        ECS::CollisionShape::BOX,
        Vec4F::One() * 0.5f);

    auto& shapes = CubeShapes[InID]; 
    for (const Vec3F& pos : InPositions)
    {
        PxShape* shape = Persistent.Physics->createShape(
           *geometry,
           *material,
           true);
        PxVec3 pxPos = Utility::PhysX::ConvertVec(pos);
        shape->setLocalPose(PxTransform(pxPos));
        CubeOwner->attachShape(*shape); 
        shapes.push_back(shape); 
    }
}

void Physics::Manager::ClearCubes(ECS::EntityID InID)
{
    const auto shapes = CubeShapes.find(InID);
    CHECK_RETURN(shapes == CubeShapes.end())
    for (PxShape* shape : shapes->second)
        shape->release();
    CubeShapes.erase(InID);
    if (CubeShapes.empty() && CubeOwner)
    {
        CubeOwner->release();
        CubeOwner = nullptr; 
    }
}
