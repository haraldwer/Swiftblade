#pragma once

#include <physx/PxForceMode.h>

#include "Engine/ECS/entity.h"
#include "Resources/Material.h"
#include "Utility/Singelton.h"

namespace ECS
{
    struct Rigidbody;
    struct Collider;
}

namespace physx
{
    struct PxForceMode;
    enum PxForceMode::Enum;
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
        void Update(double InDelta);

        void Add(ECS::EntityID InID);
        void Remove(ECS::EntityID InID);
        
        void AddForce(ECS::EntityID InID, const Vec3F& InForce, ForceMode InForceMode);

        physx::PxMaterial* CreateMaterial(float InStaticFric, float InDynamicFric, float InRestitution) const;

    private:
        
        static physx::PxForceMode::Enum ConvertForceMode(ForceMode InMode);

        physx::PxScene* Scene = nullptr;

        Map<ECS::EntityID, physx::PxActor*> Instances; 
        Map<ECS::EntityID, physx::PxRigidDynamic*> Dynamics; 
    };
}
