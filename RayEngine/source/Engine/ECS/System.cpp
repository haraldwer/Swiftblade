#include "System.h"

#include "Manager.h"
#include "Engine/Instance/Instance.h"

bool ECS::SystemBase::ShouldUpdate() const
{
    return
        !Engine::InstanceBase::Get().IsEditor() &&
        !Utility::Time::Get().IsPaused();
}

ECS::SystemBase* ECS::SystemBase::GetAnonymousSystem(const Utility::Type& InType, bool InIsCompHash)
{
    return Manager::Get().GetSystem(InType, InIsCompHash);
}

bool ECS::SystemBase::Contains(EntityID InID) const
{
    return Translation.contains(InID);
}

Vector<ECS::EntityID> ECS::SystemBase::GetEntities() const
{
    Vector<EntityID> result;
    for (auto& entry : Translation)
        result.push_back(entry.first);
    return result; 
}

ECS::ComponentID ECS::SystemBase::Translate(EntityID InID) const
{
    const auto find = Translation.find(InID);
    if (find != Translation.end())
        return find->second;
    return -1;
}
