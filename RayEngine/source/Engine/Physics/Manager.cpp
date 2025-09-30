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
            for (auto id : data)
            {
                //auto& e = data.at(id); 
                CHECK_ASSERT(!id.second.rb, "Invalid rb");
                id.second.rb->setIsDebugEnabled(false);
            }
        }
    }

    CHECK_RETURN(!draw);

    for (auto id : data)
    {
        //auto& e = data.at(id); 
        CHECK_ASSERT(!id.second.rb, "Invalid rb");
        if (!id.second.rb->isDebugEnabled())
            id.second.rb->setIsDebugEnabled(true);
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
            //rlEnableWireMode();
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
            //rlDisableWireMode();
        }

        uint32 numTris = renderer.getNbTriangles();
        if (numTris > 0)
        {
            auto* triangles = renderer.getTrianglesArray();
            //rlEnableWireMode();
            for (int i  = 0; i < static_cast<int>(numTris); i++)
            {
                rlBegin(RL_LINES);
                auto& tri = triangles[i];
                setColor(tri.color1);
                rlVertex3f(tri.point1.x, tri.point1.y, tri.point1.z);
                setColor(tri.color2);
                rlVertex3f(tri.point2.x, tri.point2.y, tri.point2.z);
                setColor(tri.color3);
                rlVertex3f(tri.point3.x, tri.point3.y, tri.point3.z);
                rlEnd(); 
            }
            //rlDisableWireMode();
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
            entity.rb->setAngularLockAxisFactor({0, 0, 0});
        
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
    Mat4F colliderLocal = rb ? colliderWorld * Mat4F::GetInverse(rbWorld) : Mat4F();
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

void Physics::Manager::AddCubes(ECS::EntityID InID, const Vector<Mat4F> &InTransforms, float InScale, const ResPM& InMat)
{
    PROFILE();
    CHECK_ASSERT(InID == ECS::INVALID_ID, "Invalid ID");
    Remove(InID);
    
    auto mat = InMat.Get();
    CHECK_RETURN_LOG(!mat, "Invalid cube material");

    const auto& ecs = ECS::Manager::Get();
    const auto t = ecs.GetComponent<ECS::Transform>(InID);
    CHECK_RETURN_LOG(!t, "No transform for entity");
    
    auto& entity = data[InID];
    
    if (!entity.rb)
    {
        entity.rb = world->createRigidBody(GetTrans(Mat4F()));
        CHECK_ASSERT(!entity.rb, "Failed to create Rigidbody");
    }
    
    entity.rb->setType(reactphysics3d::BodyType::STATIC);

    // Generate a custom mesh for the cube volume
    Vector<Vec3F> vertices;
    Vector<int> indices;
    vertices.reserve(8 * InTransforms.size());
    indices.reserve(12 * 3 * InTransforms.size());
    
    const auto addCube([&](const Mat4F& InWorld) 
    {
        Vec3F v[8] = {
            // Front face (vertices 0-3)
            { -1.0f,  1.0f,  1.0f },  // 0: Top-left-front
            {  1.0f,  1.0f,  1.0f },  // 1: Top-right-front
            {  1.0f, -1.0f,  1.0f },  // 2: Bottom-right-front
            { -1.0f, -1.0f,  1.0f },  // 3: Bottom-left-front

            // Back face (vertices 4-7)
            { -1.0f,  1.0f, -1.0f, }, // 4: Top-left-back
            {  1.0f,  1.0f, -1.0f, }, // 5: Top-right-back
            {  1.0f, -1.0f, -1.0f, }, // 6: Bottom-right-back
            { -1.0f, -1.0f, -1.0f  }, // 7: Bottom-left-back
        };

        int i[12 * 3] = {
            // Face 1 (Front)
            0, 1, 2,  // Triangle 1: Top-left, Top-right, Bottom-right
            0, 2, 3,  // Triangle 2: Top-left, Bottom-right, Bottom-left

            // Face 2 (Back)
            5, 4, 7,  // Triangle 3: Top-right, Top-left, Bottom-left (Note: Winding order is crucial)
            5, 7, 6,  // Triangle 4: Top-right, Bottom-left, Bottom-right

            // Face 3 (Top)
            4, 5, 1,  // Triangle 5
            4, 1, 0,  // Triangle 6

            // Face 4 (Bottom)
            3, 2, 6,  // Triangle 7
            3, 6, 7,  // Triangle 8

            // Face 5 (Right)
            1, 5, 6,  // Triangle 9
            1, 6, 2,  // Triangle 10

            // Face 6 (Left)
            4, 0, 3,  // Triangle 11
            4, 3, 7   // Triangle 12
        };

        const int startIndex = static_cast<int>(vertices.size());
        for (auto& vertex : v)
        {
            const Vec4F p = {
                vertex.x,
                vertex.y,
                vertex.z,
                1
            };
            vertices.push_back((InWorld * p).xyz);
        }

        for (int index : i)
            indices.push_back(index + startIndex);
    });
    
    for (auto& world : InTransforms)
        addCube(world);
    
    reactphysics3d::TriangleVertexArray arr = reactphysics3d::TriangleVertexArray(
        static_cast<int>(vertices.size()),
        vertices.data(),
        sizeof(Vec3F),
        static_cast<int>(indices.size()) / 3,
        indices.data(),
        sizeof(int),
        reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
        reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
    
    Vector<reactphysics3d::Message> results;
    auto mesh = common->createTriangleMesh(arr, results);
    for (auto& r : results)
        LOG("PhysMeshError: " + r.text);
    CHECK_ASSERT(!mesh, "Failed to create triangle mesh");
    auto shape = common->createConcaveMeshShape(mesh, GetVec(InScale));
    entity.shapes.push_back(shape);
    
    for (auto cube : InTransforms)
    {
        auto c = entity.rb->addCollider(shape, GetTrans(cube));
        CHECK_ASSERT(!c, "Failed to add collider");
        entity.colliders.push_back(c);

        // Update collider properties
        auto m = c->getMaterial();
        m.setFrictionCoefficient(mat->data.Friction);
        m.setBounciness(mat->data.Bounciness);
        m.setMassDensity(mat->data.MassDensity);
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
        Mat4F mat = trans->World();
        CHECK_CONTINUE(mat == d.second.cache)
        CHECK_ASSERT(!d.second.rb, "Invalid rb");
        CHECK_CONTINUE(d.second.rb->getType() == reactphysics3d::BodyType::STATIC);
        d.second.rb->setTransform(GetTrans(mat));
        d.second.cache = mat;
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
