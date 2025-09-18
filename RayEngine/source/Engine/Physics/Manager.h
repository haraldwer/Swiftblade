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
    class PhysicsCommon;
    class PhysicsWorld;
    class Vector3;
    class Quaternion;
    class Transform;
    class RigidBody;
}

namespace Physics
{
    enum class Shape : uint8;
    class Callback;
    
    class Manager : public Utility::Singelton<Manager>
    {
        friend class Query;
        friend class QueryFilter;
    public:

        void Init();
        void Deinit();
        void Update() const;

        void Add(ECS::EntityID InID);
        void Remove(ECS::EntityID InID);
        
        void AddCubes(ECS::EntityID InID, const Vector<Mat4F>& InTransforms, float InScale);
        void ClearCubes(ECS::EntityID InID);

        static void* CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution);

    private:
        void SetRBTransforms() const;
        void Simulate() const;
        void SetEntityTransforms() const;

        static ECS::Rigidbody* FindRigidbody(ECS::EntityID InID);
        void CreateShape(const ECS::Collider& InCollider, const ECS::EntityID& InActorID, const ECS::Transform& InTrans, reactphysics3d::RigidBody& InRB);
        static void* GetGeometry(const Shape& InShape, const Vec4F& InShapeData, const Vec3F& InScale);
        
        void TryReleaseShape(ECS::EntityID InID); 
        void TryReleaseDynamic(ECS::EntityID InID); 
        void TryReleaseStatic(ECS::EntityID InID);

        reactphysics3d::PhysicsCommon* common;
        reactphysics3d::PhysicsWorld* world;
        Map<ECS::EntityID, reactphysics3d::RigidBody*> rbs;
        
    };
}
