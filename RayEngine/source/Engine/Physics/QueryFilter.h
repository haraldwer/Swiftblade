#pragma once
#include <PxQueryFiltering.h>

#include "ECS/Entity.h"

namespace Physics
{
    class QueryFilter : public physx::PxQueryFilterCallback
    {
    public:
        void SetIgnoredEntities(const Set<ECS::EntityID>& InEntities);
        physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags) override;
        physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override;

    private:
        static void GetHierarchyEntities(ECS::EntityID InParent, Set<ECS::EntityID>& OutSet);
        Set<const physx::PxRigidActor*> IgnoredActors;
    };
}
