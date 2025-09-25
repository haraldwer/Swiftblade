#include "Manager.h"
#include "Utility.h"

#include "ECS/Manager.h"
#include "Rendering/Manager.h"

#include "ECS/Systems/Collider.h"
#include "ECS/Systems/Rigidbody.h"
#include "ECS/Systems/Transform.h"

#include "CollisionShape.h"
#include <reactphysics3d/reactphysics3d.h>

#include "Logger.h"
#include "raylib.h"
#include "rlgl.h"
#include "ECS/Volume/CubeVolume.h"
#include "Instance/Instance.h"

void Physics::Manager::Init()
{
    PROFILE();
    common = new reactphysics3d::PhysicsCommon();
    logger = new Logger();
    common->setLogger(logger);
    
    reactphysics3d::PhysicsWorld::WorldSettings settings;
    world = common->createPhysicsWorld(settings);
    CHECK_ASSERT(!world, "Failed to create world");
}

void Physics::Manager::Deinit()
{
    PROFILE();
    CHECK_ASSERT(!common, "Invalid common");
    CHECK_ASSERT(!world, "Invalid world");

    for (auto& e : data)
        DestroyData(e.second);
    data.clear();
    dynamic.clear();
    
    common->destroyPhysicsWorld(world);
    world = nullptr;
    delete common;
    common = nullptr;
    delete logger;
    logger = nullptr;
}

void Physics::Manager::Update()
{
    PROFILE();
    SetRBTransforms();
    Simulate();
    SetEntityTransforms();
}

void Physics::Manager::Frame() const
{
    PROFILE();
#ifdef _DEBUG
    bool draw = Rendering::Manager::Get().GetConfig().DrawPhysicsWorld;

    if (world->getIsDebugRenderingEnabled() != draw)
    {
        world->setIsDebugRenderingEnabled(draw);
        auto& renderer = world->getDebugRenderer();
        renderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
        renderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE_NORMAL, true);
        renderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
        renderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
        renderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
        renderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);

        if (!draw)
        {
            for (auto id : dynamic)
            {
                auto& e = data.at(id); 
                CHECK_ASSERT(!e.rb, "Invalid rb");
                e.rb->setIsDebugEnabled(false);
            }
        }
    }

    CHECK_RETURN(!draw);

    for (auto id : dynamic)
    {
        auto& e = data.at(id); 
        CHECK_ASSERT(!e.rb, "Invalid rb");
        if (!e.rb->isDebugEnabled())
            e.rb->setIsDebugEnabled(true);
    }
    
    Engine::Instance::Get().GetRenderScene().AddDebugFunc([&]()
    {
        PROFILE();
        auto& renderer = world->getDebugRenderer();
        auto setColor = [](const uint32 InC)
        {
            union U
            {
                struct
                {
                    unsigned char r;
                    unsigned char g;
                    unsigned char b;
                    unsigned char a;
                } col;
                uint32 val;
            };
            const U u { .val = InC };
            rlColor4ub(u.col.r, u.col.g, u.col.b, 255);
        };

        uint32 numLines = renderer.getNbLines();
        if (numLines > 0)
        {
            auto* lines = renderer.getLinesArray();
            for (int i = 0; i < static_cast<int>(numLines); i++)
            {
                rlBegin(RL_LINES);
                auto& line = lines[i];
                setColor(line.color1);
                rlVertex3f(line.point1.x, line.point1.y, line.point1.z);
                setColor(line.color2);
                rlVertex3f(line.point2.x, line.point2.y, line.point2.z);
                rlEnd();
            }
        }

        uint32 numTris = renderer.getNbTriangles();
        if (numTris > 0)
        {
            auto* triangles = renderer.getTrianglesArray();
            rlEnableWireMode();
            for (int i  = 0; i < static_cast<int>(numTris); i++)
            {
                rlBegin(RL_TRIANGLES);
                auto& tri = triangles[i];
                setColor(tri.color1);
                rlVertex3f(tri.point1.x, tri.point1.y, tri.point1.z);
                setColor(tri.color2);
                rlVertex3f(tri.point2.x, tri.point2.y, tri.point2.z);
                setColor(tri.color3);
                rlVertex3f(tri.point3.x, tri.point3.y, tri.point3.z);
                rlEnd(); 
            }
            rlDisableWireMode();
        }
    });
#endif
}

void Physics::Manager::Add(const ECS::EntityID InID)
{
    PROFILE();
    CHECK_ASSERT(InID == ECS::INVALID_ID, "Invalid ID");
    const auto& ecs = ECS::Manager::Get();
    const auto collider = ecs.GetComponent<ECS::Collider>(InID); 
    CHECK_RETURN_LOG(!collider, "No collider for entity");
    const auto t = ecs.GetComponent<ECS::Transform>(InID);
    CHECK_RETURN_LOG(!t, "No transform for entity");

    ECS::Rigidbody* rb = FindRigidbody(InID);
    const ECS::EntityID id = rb ? rb->GetID() : InID;

    Mat4F colliderWorld = t->World();
    Mat4F rbWorld = colliderWorld;
    if (auto rbT = ecs.GetComponent<ECS::Transform>(id))
        rbWorld = rbT->World();
    
    auto& entity = data[id];
    
    if (!entity.rb)
    {
        entity.rb = world->createRigidBody(GetTrans(rbWorld));
        CHECK_ASSERT(!entity.rb, "Failed to create Rigidbody");
    }
    
    if (rb) 
    {
        dynamic.insert(id);
        rb->ptr = entity.rb;
        entity.rb->setType(reactphysics3d::BodyType::DYNAMIC);
        entity.rb->setMass(rb->Mass);
        entity.rb->setLinearDamping(rb->LinearDamping);
        entity.rb->setAngularDamping(rb->AngularDamping);
        entity.rb->setIsAllowedToSleep(true);

        if (rb->LockRotation)
            entity.rb->setAngularLockAxisFactor({1, 1, 1});
        
        // TODO: Maybe update center of mass to match rb position?
        // physRb->setLocalCenterOfMass();
    }
    else // No rb, static object
    {
        entity.rb->setType(reactphysics3d::BodyType::STATIC);
    }

    auto shape = CreateShape(*collider, *t);
    CHECK_ASSERT(!shape, "Unknown shape");
    entity.shapes.push_back(shape);
    
    // Make collider transform relative to rb
    Mat4F colliderLocal = colliderWorld * Mat4F::GetInverse(rbWorld);
    auto c = entity.rb->addCollider(shape, GetTrans(colliderLocal));
    CHECK_ASSERT(!c, "Failed to add collider");
    entity.colliders.push_back(c);

    // Update collider properties
    c->setIsTrigger(collider->IsTrigger);
    if (auto mat = collider->Material.Get().Get())
    {
        auto m = c->getMaterial();
        m.setFrictionCoefficient(mat->data.Friction);
        m.setBounciness(mat->data.Bounciness);
        m.setMassDensity(mat->data.MassDensity);
    }
}

void Physics::Manager::AddCubes(ECS::EntityID InID, const Vector<Mat4F> &InTransforms, float InScale)
{
    PROFILE();
    CHECK_ASSERT(InID == ECS::INVALID_ID, "Invalid ID");
    Remove(InID);
    
    const auto& ecs = ECS::Manager::Get();
    const auto t = ecs.GetComponent<ECS::Transform>(InID);
    CHECK_RETURN_LOG(!t, "No transform for entity");
    const auto volume = ecs.GetComponent<ECS::CubeVolume>(InID);
    CHECK_RETURN_LOG(!volume, "No volume for entity");
    auto mat = volume->PhysicsMaterial.Get().Get();

    auto& entity = data[InID];
    
    if (!entity.rb)
    {
        entity.rb = world->createRigidBody(GetTrans(t->World()));
        CHECK_ASSERT(!entity.rb, "Failed to create Rigidbody");
    }
    
    entity.rb->setType(reactphysics3d::BodyType::STATIC);

    auto shape = common->createBoxShape({ InScale, InScale, InScale });
    CHECK_ASSERT(!shape, "Unknown shape");
    entity.shapes.push_back(shape);
    
    for (auto cube : InTransforms)
    {
        auto c = entity.rb->addCollider(shape, GetTrans(cube));
        CHECK_ASSERT(!c, "Failed to add collider");
        entity.colliders.push_back(c);

        // Update collider properties
        if (mat)
        {
            auto m = c->getMaterial();
            m.setFrictionCoefficient(mat->data.Friction);
            m.setBounciness(mat->data.Bounciness);
            m.setMassDensity(mat->data.MassDensity);
        }
    }
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

reactphysics3d::CollisionShape* Physics::Manager::CreateShape(const ECS::Collider &InCollider, const ECS::Transform &InTrans) const
{
    Vec3F scale = InTrans.GetScale();
    Vec4F shapeData = InCollider.ShapeData.Get();
    switch (static_cast<Shape>(InCollider.Shape.Get()))
    {
        case Shape::BOX:
        {
            return common->createBoxShape(GetVec(shapeData.xyz * scale));
        }
        case Shape::CAPSULE:
        {
            const float rScale = Utility::Math::Max(scale.x, scale.z);
            return common->createCapsuleShape(shapeData.x * rScale, shapeData.y * scale.y);
        }
        case Shape::SPHERE:
        {
            const float rScale = Utility::Math::Max(Utility::Math::Max(scale.x, scale.y), scale.z);
            return common->createSphereShape(shapeData.x * rScale);
        }
    }
    return nullptr;
}

void Physics::Manager::DestroyData(const Data& InData) const
{
    CHECK_ASSERT(!InData.rb, "Invalid data");
    for (auto c : InData.colliders)
        InData.rb->removeCollider(c);
    world->destroyRigidBody(InData.rb);
    for (auto s : InData.shapes)
    {
        switch (s->getName())
        {
            case reactphysics3d::CollisionShapeName::SPHERE:
                common->destroySphereShape(reinterpret_cast<reactphysics3d::SphereShape*>(s));
                break;
            case reactphysics3d::CollisionShapeName::CAPSULE:
                common->destroyCapsuleShape(reinterpret_cast<reactphysics3d::CapsuleShape*>(s));
                break;
            case reactphysics3d::CollisionShapeName::BOX:
                common->destroyBoxShape(reinterpret_cast<reactphysics3d::BoxShape*>(s));
                break;
            default:
                CHECK_ASSERT(false, "Invalid collision shape");
        }
    }
}

void Physics::Manager::Remove(ECS::EntityID InID)
{
    if (dynamic.contains(InID))
    {
        if (const auto rb = ECS::Manager::Get().GetComponent<ECS::Rigidbody>(InID))
            rb->ptr = nullptr;
        dynamic.erase(InID);
    }
    auto entity = data.find(InID);
    if (entity != data.end())
    {
        DestroyData(entity->second);
        data.erase(InID);
    }
}

void Physics::Manager::SetRBTransforms()
{
    PROFILE();
    const auto& ecs = ECS::Manager::Get();
    for (auto& d : data)
    {
        auto trans = ecs.GetComponent<ECS::Transform>(d.first);
        CHECK_ASSERT(!trans, "Object has no transform");
        Mat4F world = trans->World();
        CHECK_CONTINUE(world == d.second.cache)
        CHECK_ASSERT(!d.second.rb, "Invalid rb");
        CHECK_CONTINUE(d.second.rb->getType() == reactphysics3d::BodyType::STATIC);
        d.second.rb->setTransform(GetTrans(world));
        d.second.cache = world;
    }

    // TODO: Maybe update rb properties for dynamics?
}

void Physics::Manager::Simulate() const
{
    PROFILE();
    const double delta = Utility::Time::Get().Delta();
    world->update(static_cast<reactphysics3d::decimal>(delta));
}

void Physics::Manager::SetEntityTransforms() const
{
    PROFILE();
    const auto& ecs = ECS::Manager::Get();
    for (auto& id : dynamic)
    {
        auto& d = data.at(id);
        CHECK_ASSERT(!d.rb, "Invalid rb");
        CHECK_CONTINUE(d.rb->getType() == reactphysics3d::BodyType::STATIC);
        auto trans = ecs.GetComponent<ECS::Transform>(id);
        CHECK_ASSERT(!trans, "Object has no transform");
        trans->SetWorld(GetTrans(d.rb->getTransform()));
    }
}
