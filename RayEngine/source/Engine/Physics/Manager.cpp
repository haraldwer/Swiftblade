#include "Manager.h"

#include "ECS/Manager.h"

#include "ECS/Systems/Collider.h"
#include "ECS/Systems/Rigidbody.h"
#include "ECS/Systems/Transform.h"

// Main physx includes
#include "PhysXUtility.h"
#include "physx/PxPhysicsAPI.h"

#include "Callback.h"
#include "CollisionShape.h"

#define PVD_HOST "127.0.0.1"

static physx::PxDefaultAllocator gAllocator;
static Physics::ErrorCallback gErrorCallback;

static Physics::PersistentPhysics persistent;

using namespace physx;

void Physics::PersistentPhysics::TryInit()
{
    if (foundation)
        return; 

    Deinit();
    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    pvd = PxCreatePvd(*foundation);
    transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    pvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), false, pvd);
    dispatcher = PxDefaultCpuDispatcherCreate(0); 

    #ifdef _MSC_VER
    scratchBlock = _aligned_malloc(SCRATCH_BLOCK_SIZE, SCRATCH_BLOCK_ALIGNMENT);
    #endif
}

void Physics::PersistentPhysics::Deinit()
{
    PX_RELEASE(dispatcher);
    PX_RELEASE(physics);
    if(pvd)
    {
        if (pvd->isConnected())
            pvd->disconnect();
        PX_RELEASE(pvd);
    }
    PX_RELEASE(transport);
    PX_RELEASE(foundation);
    if (scratchBlock)
    {
#ifdef _MSC_VER
        _aligned_free(scratchBlock);
#else
        free(scratchBlock);
#endif
        scratchBlock = nullptr; 
    }
}

Physics::PersistentPhysics::~PersistentPhysics()
{
    Deinit();
}

void Physics::Manager::Init()
{
    persistent.TryInit();

    if (!callback)
        callback = new Physics::Callback();
    
    PxSceneDesc sceneDesc(persistent.physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f); // Apply gravity manually
    sceneDesc.cpuDispatcher	= persistent.dispatcher;
    sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eSUPPRESS;
    sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eSUPPRESS;
    sceneDesc.filterShader = Callback::contactReportFilterShader;
    sceneDesc.simulationEventCallback = callback; 
    
    scene = persistent.physics->createScene(sceneDesc);

    if(PxPvdSceneClient* pvdClient = scene->getScenePvdClient())
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

void Physics::Manager::Deinit()
{
    actorToShape.clear();
    shapeToActor.clear();
    for (auto& shape : shapes)
        PX_RELEASE(shape.second);
    shapes.clear();
    for (auto& cube : cubeShapes)
        for (auto& shape : cube.second)
            PX_RELEASE(shape);
    cubeShapes.clear();
    PX_RELEASE(cubeOwner);
    PX_RELEASE(scene);
    if (callback)
    {
        delete(callback);
        callback = nullptr;
    }
}

void Physics::Manager::SetTransforms() const
{
    PROFILE();
    const ECS::Manager& ecs = ECS::Manager::Get();
    const auto updatePhysTrans = [](const ECS::Manager& InECS, const ECS::EntityID InKey, auto* InInstance)
    {
        CHECK_RETURN(!InInstance)
        const auto* t = InECS.GetComponent<ECS::Transform>(InKey);
        CHECK_RETURN(!t);
        const PxTransform trans = PxTransform(
            Utility::PhysX::ConvertVec(t->GetPosition()),
            Utility::PhysX::ConvertQuat(t->GetRotation()));
        if (InInstance->getGlobalPose() != trans)
            InInstance->setGlobalPose(trans);
    };
    
    for (const auto& stat : statics)
        updatePhysTrans(ecs, stat.first, stat.second);
    for (const auto& dyn : dynamics)
        updatePhysTrans(ecs, dyn.first, dyn.second);
}

void Physics::Manager::Simulate() const
{
    PROFILE();
    const double delta = Utility::Time::Get().Delta();
    scene->simulate(
        static_cast<PxReal>(delta),
        nullptr,
        persistent.scratchBlock,
        persistent.scratchBlock ?
            PersistentPhysics::SCRATCH_BLOCK_SIZE : 0);
    PxU32 error = 0;
    scene->fetchResults(true, &error);
    CHECK_ASSERT(error, "PxFetchResults error");
}

void Physics::Manager::GetTransforms() const
{
    PROFILE();
    const ECS::Manager& ecs = ECS::Manager::Get();
    for (const auto& dyn : dynamics)
    {
        CHECK_CONTINUE(!dyn.second)
        auto* t = ecs.GetComponent<ECS::Transform>(dyn.first);
        CHECK_CONTINUE(!t);
        PxTransform trans = dyn.second->getGlobalPose();
        const Vec3F p = Utility::PhysX::ConvertVec(trans.p);
        const QuatF q = Utility::PhysX::ConvertQuat(trans.q);
        const Mat4F mat = Mat4F(p, q, t->GetScale());
        t->SetWorld(mat); 
    }
}

void Physics::Manager::Update() const
{
    PROFILE();
    SetTransforms();
    Simulate();
    GetTransforms();
}

void Physics::Manager::Add(const ECS::EntityID InID)
{
    const auto& ecs = ECS::Manager::Get();
    const auto collider = ecs.GetComponent<ECS::Collider>(InID); 
    CHECK_RETURN_LOG(!collider, "No collider for entity");
    
    ECS::EntityID actorID = ECS::INVALID_ID;
    PxRigidActor* actor = nullptr;
    
    if (ECS::Rigidbody* rb = FindRigidbody(InID)) // Find rigidbody in hierarchy
    {
        const ECS::EntityID rbID = rb->GetID(); 
        PxRigidDynamic*& ptr = dynamics[rbID];
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

    CHECK_RETURN_LOG(actorID == ECS::INVALID_ID, "ActorID invalid");
    CHECK_RETURN_LOG(!actor, "Actor invalid");
    CreateShape(*collider, actorID, *actor);     
}

ECS::Rigidbody* Physics::Manager::FindRigidbody(const ECS::EntityID InID)
{
    const auto& ecs = ECS::Manager::Get();
    ECS::EntityID currID = InID;
    while (currID != ECS::INVALID_ID)
    {
        if (const auto currRB = ecs.GetComponent<ECS::Rigidbody>(currID))
            return currRB;
        if (const auto currTrans = ecs.GetComponent<ECS::Transform>(currID))
            currID = currTrans->GetParent();
    }
    return nullptr; 
}

PxRigidDynamic* Physics::Manager::CreateDynamic(ECS::Rigidbody& InRigidbody)
{
    const auto& ecs = ECS::Manager::Get();
    const auto trans = ecs.GetComponent<ECS::Transform>(InRigidbody.GetID());
    CHECK_RETURN_LOG(!trans, "No transform", nullptr);

    const PxTransform pose = PxTransform(
            Utility::PhysX::ConvertVec(trans->GetPosition()),
            Utility::PhysX::ConvertQuat(trans->GetRotation()));
    
    PxRigidDynamic* ptr = persistent.physics->createRigidDynamic(pose);
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

    InRigidbody.ptr = ptr; 
    dynamics[InRigidbody.GetID()] = ptr;
    scene->addActor(*ptr);
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
        
    PxRigidStatic* ptr = persistent.physics->createRigidStatic(pose);
    CHECK_RETURN_LOG(!ptr, "Failed to create static", nullptr);
    statics[InCollider.GetID()] = ptr;
    scene->addActor(*ptr);
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
        static_cast<Shape>(InCollider.Shape.Get()),
        InCollider.ShapeData.Get(),
        trans->GetScale(ECS::Transform::Space::LOCAL));
    
    // Set flags
    const PxShapeFlags flags =
        PxShapeFlag::eVISUALIZATION |
        PxShapeFlag::eSCENE_QUERY_SHAPE |
        (InCollider.IsTrigger ?
            PxShapeFlag::eTRIGGER_SHAPE :
            PxShapeFlag::eSIMULATION_SHAPE); 
    
    // Create shape
    PxShape* shape = persistent.physics->createShape(
        *geometry,
        *material,
        true,
        flags);
    
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
    shapes[id] = shape;
    shapeToActor[id] = InActorID;
    actorToShape[InActorID].insert(id);
}

void Physics::Manager::Remove(const ECS::EntityID InID)
{
    TryReleaseShape(InID); 
    TryReleaseStatic(InID); 
}

void Physics::Manager::TryReleaseShape(ECS::EntityID InID)
{
    const auto shapeFind = shapes.find(InID);
    CHECK_RETURN(shapeFind == shapes.end());
    
    // Release shape 
    PxShape* shape = shapeFind->second;
    if (PxRigidActor* actor = shape->getActor())
        actor->detachShape(*shape);
    shape->release();
    shapes.erase(InID);

    // Remove attachment data
    const auto rbFind = shapeToActor.find(InID); 
    CHECK_RETURN(rbFind == shapeToActor.end());
    const ECS::EntityID rbID = rbFind->second;

    // Remove from shape -> actor map
    shapeToActor.erase(InID);

    // Remove from actor -> shape set 
    const auto rbAttachments = actorToShape.find(rbID);
    CHECK_RETURN(rbAttachments == actorToShape.end());
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
    CHECK_RETURN(!dynamics.contains(InID));
    dynamics[InID]->release();
    dynamics.erase(InID);
}

void Physics::Manager::TryReleaseStatic(ECS::EntityID InID)
{
    CHECK_RETURN(!statics.contains(InID)); 
    statics[InID]->release();
    statics.erase(InID); 
}

PxMaterial* Physics::Manager::CreateMaterial(const float InStaticFric, const float InDynamicFric, const float InRestitution)
{
    return persistent.physics->createMaterial(InStaticFric, InDynamicFric, InRestitution);
}

PxGeometry* Physics::Manager::GetGeometry(const Shape& InShape, const Vec4F& InShapeData, const Vec3F& InScale)
{
    // TODO: Use transform scale
    
    switch (InShape)
    {
    case Shape::BOX:
        {
            static PxBoxGeometry box;
            box = PxBoxGeometry(InShapeData.x * InScale.x, InShapeData.y * InScale.y, InShapeData.z * InScale.z);
            return &box; 
        }
    case Shape::CAPSULE:
        {
            static PxCapsuleGeometry capsule;
            const float rScale = Utility::Math::Max(InScale.x, InScale.z);
            capsule = PxCapsuleGeometry(InShapeData.x * rScale, InShapeData.y * InScale.y);
            return &capsule;
        }
    case Shape::SPHERE:
        {
            static PxSphereGeometry sphere;
            const float rScale = Utility::Math::Max(Utility::Math::Max(InScale.x, InScale.y), InScale.z);
            sphere = PxSphereGeometry(InShapeData.x * rScale);
            return &sphere;
        }
        // Consider cooking geometry
    }
    return nullptr;
}

void Physics::Manager::AddCubes(ECS::EntityID InID, const Vector<Mat4F>& InTransforms, const float InScale)
{
    // Create cube owner
    if (!cubeOwner)
    {
        const PxTransform pose = PxTransform(
            Utility::PhysX::ConvertVec(Vec3F::Zero()),
            Utility::PhysX::ConvertQuat(QuatF::Identity()));
        cubeOwner = persistent.physics->createRigidStatic(pose);
        CHECK_RETURN_LOG(!cubeOwner, "Failed to create cube actor");
        scene->addActor(*cubeOwner);
    }
    
    // Get material
    const PxMaterial* material = nullptr;
    if (const PhysicsMaterialResource* matRsc = ResPM("Defaults/PM_Default.json").Get())
        material = matRsc->Get();
    CHECK_RETURN_LOG(!material, "Failed to get material");

    // Get geometry
    const PxGeometry* geometry = GetGeometry(
        Shape::BOX,
        Vec4F::One(),
        InScale);

    auto& cubes = cubeShapes[InID]; 
    for (const Mat4F& trans : InTransforms)
    {
        PxShape* shape = persistent.physics->createShape(
           *geometry,
           *material,
           true);
        shape->userData = ECS::EntityToPtr(InID);
        const PxTransform pose = PxTransform(
            Utility::PhysX::ConvertVec(trans.GetPosition()),
            Utility::PhysX::ConvertQuat(trans.GetRotation()));
        shape->setLocalPose(pose);
        cubeOwner->attachShape(*shape); 
        cubes.push_back(shape); 
    }
}

void Physics::Manager::ClearCubes(ECS::EntityID InID)
{
    const auto cubes = cubeShapes.find(InID);
    CHECK_RETURN(cubes == cubeShapes.end())
    for (PxShape* shape : cubes->second)
        shape->release();
    cubeShapes.erase(InID);
    if (cubeShapes.empty() && cubeOwner)
    {
        cubeOwner->release();
        cubeOwner = nullptr; 
    }
}
