#include "Manager.h"
#include "Utility.h"

#include "ECS/Manager.h"

#include "ECS/Systems/Collider.h"
#include "ECS/Systems/Rigidbody.h"
#include "ECS/Systems/Transform.h"

#include "CollisionShape.h"
#include <reactphysics3d/reactphysics3d.h>

void Physics::Manager::Init()
{
    PROFILE();
    common = new reactphysics3d::PhysicsCommon();
    reactphysics3d::PhysicsWorld::WorldSettings settings;
    world = common->createPhysicsWorld(settings);
    CHECK_ASSERT(!world, "Failed to create world");
}

void Physics::Manager::Deinit()
{
    PROFILE();
    CHECK_ASSERT(!common, "Invalid common");
    CHECK_ASSERT(!world, "Invalid world");
    
    // TODO: Destroy all shapes and rbs
    for (auto& rb : rbs)
        world->destroyRigidBody(rb.second);
    
    common->destroyPhysicsWorld(world);
    world = nullptr;
    delete common;
    common = nullptr;
}

void Physics::Manager::Update() const
{
    PROFILE();
    SetRBTransforms();
    Simulate();
    SetEntityTransforms();
}

void Physics::Manager::Add(const ECS::EntityID InID)
{
    CHECK_ASSERT(InID == ECS::INVALID_ID, "Invalid ID");
    const auto& ecs = ECS::Manager::Get();
    const auto collider = ecs.GetComponent<ECS::Collider>(InID); 
    CHECK_RETURN_LOG(!collider, "No collider for entity");
    const auto t = ecs.GetComponent<ECS::Transform>(InID);
    CHECK_RETURN_LOG(!t, "No transform for entity");

    const ECS::Rigidbody* rb = FindRigidbody(InID);
    ECS::EntityID actorID = rb ? rb->GetID() : InID;
    
    auto& physRb = rbs[actorID];
    if (!physRb)
    {
        physRb = world->createRigidBody(GetTrans(t->World()));
        CHECK_ASSERT(!physRb, "Failed to create Rigidbody");
    }
    
    if (rb) 
    {
        physRb->setType(reactphysics3d::BodyType::DYNAMIC);
        physRb->setMass(rb->Mass);
        physRb->setLinearDamping(rb->LinearDamping);
        physRb->setAngularDamping(rb->AngularDamping);
        physRb->setIsAllowedToSleep(true);

        // TODO: Maybe update center of mass to match rb position?
        // physRb->setLocalCenterOfMass();
    }
    else // No rb, static object
    {
        physRb->setType(reactphysics3d::BodyType::STATIC);
    }

    CreateShape(*collider, actorID, *t, *physRb);
}

ECS::Rigidbody* Physics::Manager::FindRigidbody(ECS::EntityID InID)
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

void Physics::Manager::CreateShape(const ECS::Collider &InCollider, const ECS::EntityID &InActorID, const ECS::Transform &InTrans, reactphysics3d::RigidBody &InRB)
{
    reactphysics3d::CollisionShape* shape = nullptr;
    Vec3F scale = InTrans.GetScale();
    Vec4F shapeData = InCollider.ShapeData.Get();
    switch (static_cast<Shape>(InCollider.Shape.Get()))
    {
        case Shape::BOX:
        {
            shape = common->createBoxShape(GetVec(shapeData.xyz * scale));
            break;
        }
        case Shape::CAPSULE:
        {
            const float rScale = Utility::Math::Max(scale.x, scale.z);
            shape = common->createCapsuleShape(shapeData.x * rScale, shapeData.y * scale.y);
            break;
        }
        case Shape::SPHERE:
        {
            const float rScale = Utility::Math::Max(Utility::Math::Max(scale.x, scale.y), scale.z);
            shape = common->createSphereShape(shapeData.x * rScale);
            break;
        }
    }

    CHECK_ASSERT(!shape, "Unknown shape");
    InRB.addCollider(shape, GetTrans(InTrans.World()));
}

void Physics::Manager::Remove(ECS::EntityID InID)
{
    auto rb = rbs.find(InID);
    if (rb != rbs.end())
    {
        world->destroyRigidBody(rb->second);
        rbs.erase(InID);
    }
}

void Physics::Manager::AddCubes(ECS::EntityID InID, const Vector<Mat4F> &InTransforms, float InScale)
{
    const auto& ecs = ECS::Manager::Get();
    const auto collider = ecs.GetComponent<ECS::Collider>(InID); 
    CHECK_RETURN_LOG(!collider, "No collider for entity");
    const auto t = ecs.GetComponent<ECS::Transform>(InID);
    CHECK_RETURN_LOG(!t, "No transform for entity");
    
    ECS::EntityID actorID = ECS::INVALID_ID;
    auto& physRb = rbs[actorID];
    if (!physRb)
    {
        physRb = world->createRigidBody(GetTrans(t->World()));
        CHECK_ASSERT(!physRb, "Failed to create Rigidbody");
    }
    
    actorID = InID;
    physRb->setType(reactphysics3d::BodyType::STATIC);

    CHECK_ASSERT(actorID == ECS::INVALID_ID, "ActorID invalid");
    CreateShape(*collider, actorID, *t, *physRb);
}

void Physics::Manager::ClearCubes(ECS::EntityID InID)
{
    
}

void* Physics::Manager::CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution)
{
    return nullptr;
}

void Physics::Manager::SetRBTransforms() const
{
    const auto& ecs = ECS::Manager::Get();
    for (auto& rb : rbs)
    {
        CHECK_ASSERT(!rb.second, "Invalid rb");
        CHECK_CONTINUE(rb.second->getType() == reactphysics3d::BodyType::STATIC);
        auto trans = ecs.GetComponent<ECS::Transform>(rb.first);
        CHECK_ASSERT(!trans, "Object has no transform");
        rb.second->setTransform(GetTrans(trans->World()));
    }
}

void Physics::Manager::Simulate() const
{
    const double delta = Utility::Time::Get().Delta();
    world->update(static_cast<reactphysics3d::decimal>(delta));
}

void Physics::Manager::SetEntityTransforms() const
{
    const auto& ecs = ECS::Manager::Get();
    for (auto& rb : rbs)
    {
        CHECK_ASSERT(!rb.second, "Invalid rb");
        CHECK_CONTINUE(rb.second->getType() == reactphysics3d::BodyType::STATIC);
        auto trans = ecs.GetComponent<ECS::Transform>(rb.first);
        CHECK_ASSERT(!trans, "Object has no transform");
        trans->SetWorld(GetTrans(rb.second->getTransform()));
    }
}
