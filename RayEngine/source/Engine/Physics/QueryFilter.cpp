#include "QueryFilter.h"

#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>

#include "Manager.h"
#include "ECS/Systems/Transform.h"

void Physics::QueryFilter::SetIgnoredEntities(const Set<ECS::EntityID>& InEntities)
{
    ignoredActors.clear();
    auto& man = Manager::Get();

    Set<ECS::EntityID> entities;
    for (const ECS::EntityID entity : InEntities)
        GetHierarchyEntities(entity, entities); 
    
    for (auto e : entities)
    {
        auto dynamicFind = man.dynamics.find(e);
        if (dynamicFind != man.dynamics.end())
            if (dynamicFind->second)
                ignoredActors.insert(dynamicFind->second);

        auto staticFind = man.statics.find(e);
        if (staticFind != man.statics.end())
            if (staticFind->second)
                ignoredActors.insert(staticFind->second);
    }
}

physx::PxQueryHitType::Enum Physics::QueryFilter::preFilter(const physx::PxFilterData& InFilterData, const physx::PxShape* InShape, const physx::PxRigidActor* InActor, physx::PxHitFlags& InQueryFlags)
{
    if (ignoredActors.contains(InActor))
        return physx::PxQueryHitType::eNONE;
    return physx::PxQueryHitType::eTOUCH;
}

physx::PxQueryHitType::Enum Physics::QueryFilter::postFilter(const physx::PxFilterData& InFilterData, const physx::PxQueryHit& InHit, const physx::PxShape* InShape, const physx::PxRigidActor* InActor)
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
