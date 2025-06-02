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
    return translation.contains(InID);
}

Vector<ECS::EntityID> ECS::SystemBase::GetEntities() const
{
    Vector<EntityID> result;
    for (auto& entry : translation)
        result.push_back(entry.first);
    return result; 
}

ECS::ComponentID ECS::SystemBase::Translate(EntityID InID) const
{
    const auto find = translation.find(InID);
    if (find != translation.end())
        return find->second;
    return -1;
}
