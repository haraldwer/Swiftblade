#include "System.h"

#include "Manager.h"
#include "Instance/Instance.h"

bool ECS::SystemBase::ShouldUpdate() const
{
    return
        !Engine::Instance::Get().IsEditor() &&
        !Utility::Time::Get().IsPaused();
}

ECS::SystemBase* ECS::SystemBase::GetAnonymousSystem(const Utility::Type& InType, const bool InIsCompHash)
{
    return Manager::Get().GetSystem(InType, InIsCompHash);
}

bool ECS::SystemBase::Contains(EntityID InID) const
{
    return entityToComponent.contains(InID);
}

Vector<ECS::EntityID> ECS::SystemBase::GetEntities() const
{
    Vector<EntityID> result;
    for (auto& entry : entityToComponent)
        result.push_back(entry.first);
    return result; 
}

ECS::ComponentID ECS::SystemBase::Translate(const EntityID InID) const
{
    const auto find = entityToComponent.find(InID);
    if (find != entityToComponent.end())
        return find->second;
    return -1;
}

ECS::ComponentID ECS::SystemBase::RegisterID(const EntityID InID)
{
    ComponentID id;
    if (!unused.empty())
    {
        id = *unused.begin();
        unused.erase(unused.begin());
    }
    else
    {
        id = NewID();
    }

    entityToComponent[InID] = id;
    componentToEntity[id] = InID;
    return id;
}

void ECS::SystemBase::UnregisterID(const EntityID InID, const ComponentID InCompID)
{
    entityToComponent.erase(InID);
    componentToEntity.erase(InCompID);
    unused.insert(InCompID);
}
