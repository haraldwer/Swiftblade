#pragma once

#include "Engine/ECS/entity.h"
#include "Utility/Singelton.h"

namespace ECS
{
    struct Rigidbody;
    struct Collider;
}

namespace physx
{
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
    class Manager : public Utility::Singelton<Manager>
    {
    public:

        void Init();
        void Deinit();
        void Update(double InDelta);

        void Add(ECS::EntityID InID);
        void Remove(ECS::EntityID InID);

    private:
        
        physx::PxFoundation* Foundation = nullptr;
        physx::PxPvd* PVD = nullptr;
        physx::PxPhysics* Physics = nullptr;
        physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
        physx::PxScene* Scene = nullptr;
        physx::PxMaterial* Material = nullptr;

        Map<ECS::EntityID, physx::PxActor*> Instances; 
        Map<ECS::EntityID, physx::PxRigidDynamic*> Rigidbodies; 
    };
}
