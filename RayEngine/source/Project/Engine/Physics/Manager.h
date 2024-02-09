#pragma once

#include "Engine/ECS/entity.h"
#include "Engine/ECS/Manager.h"
#include "Utility/Singelton.h"

namespace Physics
{
    enum class Shape : uint8;
}

namespace Physics
{
    class Callback;
}

namespace ECS
{
    struct Rigidbody;
    struct Collider;
}

namespace physx
{
    class PxRigidActor;
    class PxShape;
    class PxGeometry;
    class PxRigidStatic;
    struct PxForceMode;
    class PxActor;
    class PxRigidDynamic;
    class PxFoundation;
    class PxPvd;
    class PxPhysics;
    class PxDefaultCpuDispatcher;
    class PxScene;
    class PxMaterial;
}

namespace Physics
{
    struct PersistentPhysics
    {
        void TryInit();
        ~PersistentPhysics();

        physx::PxPvd* PVD = nullptr;
        physx::PxFoundation* Foundation = nullptr;
        physx::PxPhysics* Physics = nullptr;
        physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;

        static constexpr size_t ScratchBlockSize = 16384 * 2;
        static constexpr size_t ScratchBlockAlignment = 16;
        void* ScratchBlock = nullptr;
    };
    
    class Manager : public Utility::Singelton<Manager>
    {
        friend class Query;
        friend class QueryFilter;
    public:

        void Init();
        void Deinit();
        void Update(double InDelta) const;

        void Add(ECS::EntityID InID);
        void Remove(ECS::EntityID InID);
        
        void AddCubes(ECS::EntityID InID, const Vector<Mat4F>& InTransforms, float InScale);
        void ClearCubes(ECS::EntityID InID);

        static physx::PxMaterial* CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution);

    private:
        static ECS::Rigidbody* FindRigidbody(ECS::EntityID InID);
        physx::PxRigidDynamic* CreateDynamic(ECS::Rigidbody& InRigidbody);
        physx::PxRigidStatic* CreateStatic(const ECS::Collider& InCollider);
        void CreateShape(const ECS::Collider& InCollider, ECS::EntityID InActorID, physx::PxRigidActor& InActor);
        static physx::PxGeometry* GetGeometry(const Shape& InShape, const Vec4F& InShapeData, const Vec3F& InScale);
        
        void TryReleaseShape(ECS::EntityID InID); 
        void TryReleaseDynamic(ECS::EntityID InID); 
        void TryReleaseStatic(ECS::EntityID InID); 

        physx::PxScene* Scene = nullptr;
        Callback* Callback = nullptr; 

        Map<ECS::EntityID, physx::PxRigidStatic*> Statics; 
        Map<ECS::EntityID, physx::PxRigidDynamic*> Dynamics;
        
        Map<ECS::EntityID, physx::PxShape*> Shapes; 
        Map<ECS::EntityID, Set<ECS::EntityID>> ActorToShape; 
        Map<ECS::EntityID, ECS::EntityID> ShapeToActor;

        physx::PxRigidStatic* CubeOwner = nullptr;
        Map<ECS::EntityID, Vector<physx::PxShape*>> CubeShapes;
    };
}
