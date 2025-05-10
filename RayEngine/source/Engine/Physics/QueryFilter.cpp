#include "QueryFilter.h"

#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>

#include "Manager.h"
#include "ECS/Systems/Transform.h"

void Physics::QueryFilter::SetIgnoredEntities(const Set<ECS::EntityID>& InEntities)
{
    IgnoredActors.clear();
    auto& man = Manager::Get();

    Set<ECS::EntityID> entities;
    for (const ECS::EntityID entity : InEntities)
        GetHierarchyEntities(entity, entities); 
    
    for (auto e : entities)
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

void Physics::QueryFilter::GetHierarchyEntities(const ECS::EntityID InParent, Set<ECS::EntityID>& OutSet)
{
    OutSet.insert(InParent);
    const auto& man = ECS::Manager::Get();
    const auto trans = man.GetComponent<ECS::Transform>(InParent);
    CHECK_ASSERT(!trans, "Could not find transform for entity");
    for (const ECS::EntityID child : trans->GetChildren())
        GetHierarchyEntities(child, OutSet); 
}
