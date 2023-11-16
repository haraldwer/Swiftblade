#pragma once

#include "TraceResult.h"
#include "Engine/ECS/entity.h"
#include "Engine/ECS/Manager.h"
#include "Utility/Singelton.h"

namespace Physics
{
    class Callback;
}

namespace ECS
{
    struct Rigidbody;
    struct Collider;
    enum class CollisionShape : uint8;
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
    enum class ForceMode
    {
        FORCE,
        IMPULSE,
        VELOCITY
    };

    struct PersistentPhysics
    {
        void TryInit();
        ~PersistentPhysics();

        physx::PxPvd* PVD = nullptr;
        physx::PxFoundation* Foundation = nullptr;
        physx::PxPhysics* Physics = nullptr;
        physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
    };
    
    class Manager : public Utility::Singelton<Manager>
    {
    public:

        void Init();
        void Deinit();
        void Update(double InDelta) const;

        void Add(ECS::EntityID InID);
        void Remove(ECS::EntityID InID);
        
        void AddForce(ECS::EntityID InID, const Vec3F& InForce, ForceMode InForceMode);
        void ClearForces(ECS::EntityID InID);
        void SetKinematic(ECS::EntityID InID, bool InKinematic);
        
        void AddCubes(ECS::EntityID InID, const Vector<Vec3F>& InPositions);
        void ClearCubes(ECS::EntityID InID);

        static physx::PxMaterial* CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution);

        TraceResult Trace(const Vec3F& aStart, const Vec3F& anEnd) const;

    private:
        static ECS::Rigidbody* FindRigidbody(ECS::EntityID InID);
        physx::PxRigidDynamic* CreateDynamic(const ECS::Rigidbody& InRigidbody);
        physx::PxRigidStatic* CreateStatic(const ECS::Collider& InCollider);
        void CreateShape(const ECS::Collider& InCollider, ECS::EntityID InActorID, physx::PxRigidActor& InActor);
        static physx::PxGeometry* GetGeometry(ECS::CollisionShape InShape, const Vec4F& InShapeData);
        
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
