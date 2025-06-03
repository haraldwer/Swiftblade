#pragma once
#include <PxQueryFiltering.h>

#include "ECS/Entity.h"

namespace Physics
{
    class QueryFilter : public physx::PxQueryFilterCallback
    {
    public:
        void SetIgnoredEntities(const Set<ECS::EntityID>& InEntities);
        physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& InFilterData, const physx::PxShape* InShape, const physx::PxRigidActor* InActor, physx::PxHitFlags& InQueryFlags) override;
        physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& InFilterData, const physx::PxQueryHit& InHit, const physx::PxShape* InShape, const physx::PxRigidActor* InActor) override;

    private:
        static void GetHierarchyEntities(ECS::EntityID InParent, Set<ECS::EntityID>& OutSet);
        Set<const physx::PxRigidActor*> ignoredActors;
    };
}
