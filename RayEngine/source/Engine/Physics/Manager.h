#pragma once

#include "ECS/Entity.h"
#include "Core/Utility/Singelton.h"
#include "ECS/Systems/Transform.h"

namespace ECS
{
    struct Rigidbody;
    struct Collider;
}

namespace reactphysics3d
{
    class Collider;
    class CollisionShape;
    class PhysicsWorld;
    class PhysicsCommon;
    class RigidBody;
}

namespace Physics
{
    class Logger;
    enum class Shape : uint8;
    class Callback;
    
    class Manager : public Utility::Singelton<Manager>
    {
        friend class Query;
        friend class QueryFilter;
    public:

        void Init();
        void Deinit();
        void Update();
        void Frame() const;

        void Add(ECS::EntityID InID);
        void AddCubes(ECS::EntityID InID, const Vector<Mat4F>& InTransforms, float InScale);
        void Remove(ECS::EntityID InID);

    private:
        void SetRBTransforms();
        void Simulate() const;
        void SetEntityTransforms() const;

        static ECS::Rigidbody* FindRigidbody(ECS::EntityID InID);
        reactphysics3d::CollisionShape* CreateShape(const ECS::Collider &InCollider, const ECS::Transform &InTrans) const;
        
        reactphysics3d::PhysicsCommon* common = nullptr;
        reactphysics3d::PhysicsWorld* world = nullptr;
        Logger* logger = nullptr;
        
        struct Data
        {
            Mat4F cache;
            reactphysics3d::RigidBody* rb = nullptr;
            Vector<reactphysics3d::Collider*> colliders;
            Vector<reactphysics3d::CollisionShape*> shapes;
        };
        Map<ECS::EntityID, Data> data;
        Set<ECS::EntityID> dynamic;

        void DestroyData(const Data& InData) const;
    };
}
