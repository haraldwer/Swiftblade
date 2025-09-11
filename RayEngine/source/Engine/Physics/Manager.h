#pragma once

#include "ECS/Entity.h"
#include "ECS/Manager.h"
#include "Core/Utility/Singelton.h"

namespace ECS
{
    struct Rigidbody;
    struct Collider;
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
        void SetTransforms() const;
        void Simulate() const;
        void GetTransforms() const;

        static ECS::Rigidbody* FindRigidbody(ECS::EntityID InID);
        void* CreateDynamic(ECS::Rigidbody& InRigidbody);
        void* CreateStatic(const ECS::Collider& InCollider);
        void CreateShape(const ECS::Collider& InCollider, ECS::EntityID InActorID, int& InActor);
        static void* GetGeometry(const Shape& InShape, const Vec4F& InShapeData, const Vec3F& InScale);
        
        void TryReleaseShape(ECS::EntityID InID); 
        void TryReleaseDynamic(ECS::EntityID InID); 
        void TryReleaseStatic(ECS::EntityID InID); 
    };
}
