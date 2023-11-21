#include "QueryFilter.h"

#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxrigidStatic.h>

#include "Manager.h"

void Physics::QueryFilter::SetIgnoredEntities(const Set<ECS::EntityID>& InEntities)
{
    IgnoredActors.clear();
    auto& man = Manager::Get();
    for (auto e : InEntities)
    {
        auto dynamicFind = man.Dynamics.find(e);
        if (dynamicFind != man.Dynamics.end())
            if (dynamicFind->second)
                IgnoredActors.insert(dynamicFind->second);

        auto staticFind = man.Statics.find(e);
        if (staticFind != man.Statics.end())
            if (staticFind->second)
                IgnoredActors.insert(staticFind->second);
    }
}

physx::PxQueryHitType::Enum Physics::QueryFilter::preFilter(const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags)
{
    if (IgnoredActors.contains(actor))
        return physx::PxQueryHitType::eNONE;
    return physx::PxQueryHitType::eTOUCH;
}

physx::PxQueryHitType::Enum Physics::QueryFilter::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor)
{
    return physx::PxQueryHitType::eTOUCH;
}
