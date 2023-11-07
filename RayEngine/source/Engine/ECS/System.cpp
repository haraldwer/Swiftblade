#include "System.h"

#include "Manager.h"
#include "Engine/Instance/Instance.h"

bool ECS::SystemBase::ShouldUpdate() const
{
    return !Engine::InstanceBase::Get().IsEditor();
}

ECS::SystemBase* ECS::SystemBase::GetAnonymousSystem(size_t InHash, bool InIsCompHash) const
{
    return Manager::Get().GetSystem(InHash, InIsCompHash);
}

bool ECS::SystemBase::Contains(EntityID InID) const
{
    return Translation.contains(InID);
}

ECS::ComponentID ECS::SystemBase::Translate(EntityID InID) const
{
    const auto find = Translation.find(InID);
    if (find != Translation.end())
        return find->second;
    return -1;
}
