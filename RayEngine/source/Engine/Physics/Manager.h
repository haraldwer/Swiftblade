#pragma once

#include "ECS/Entity.h"
#include "ECS/Manager.h"
#include "Core/Utility/Singelton.h"

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
    class PxPvdTransport;
}

namespace Physics
{

    struct PersistentPhysics
    {
        void TryInit();
        void Deinit();
        ~PersistentPhysics();

        physx::PxFoundation* foundation = nullptr;
        physx::PxPvd* pvd = nullptr;
        physx::PxPhysics* physics = nullptr;
        physx::PxDefaultCpuDispatcher* dispatcher = nullptr;
        physx::PxPvdTransport* transport = nullptr;

        static constexpr size_t SCRATCH_BLOCK_SIZE = 16384 * 2;
        static constexpr size_t SCRATCH_BLOCK_ALIGNMENT = 16;
        void* scratchBlock = nullptr;
    };
    
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

        static physx::PxMaterial* CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution);

    private:
        void SetTransforms() const;
        void Simulate() const;
        void GetTransforms() const;

        static ECS::Rigidbody* FindRigidbody(ECS::EntityID InID);
        physx::PxRigidDynamic* CreateDynamic(ECS::Rigidbody& InRigidbody);
        physx::PxRigidStatic* CreateStatic(const ECS::Collider& InCollider);
        void CreateShape(const ECS::Collider& InCollider, ECS::EntityID InActorID, physx::PxRigidActor& InActor);
        static physx::PxGeometry* GetGeometry(const Shape& InShape, const Vec4F& InShapeData, const Vec3F& InScale);
        
        void TryReleaseShape(ECS::EntityID InID); 
        void TryReleaseDynamic(ECS::EntityID InID); 
        void TryReleaseStatic(ECS::EntityID InID); 

        physx::PxScene* scene = nullptr;
        Callback* callback = nullptr; 

        Map<ECS::EntityID, physx::PxRigidStatic*> statics = {}; 
        Map<ECS::EntityID, physx::PxRigidDynamic*> dynamics = {};
        
        Map<ECS::EntityID, physx::PxShape*> shapes = {}; 
        Map<ECS::EntityID, Set<ECS::EntityID>> actorToShape = {}; 
        Map<ECS::EntityID, ECS::EntityID> shapeToActor = {};

        physx::PxRigidStatic* cubeOwner = nullptr;
        Map<ECS::EntityID, Vector<physx::PxShape*>> cubeShapes = {};
    };
}
