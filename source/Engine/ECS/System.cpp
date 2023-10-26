#include "System.h"

#include "Manager.h"

ECS::SystemBase* ECS::SystemBase::GetAnonymousSystem(size_t InHash, bool InIsCompHash) const
{
    return Manager::Get().GetSystem(InHash, InIsCompHash);
}

ECS::ComponentID ECS::SystemBase::Translate(EntityID InEntity) const
{
    const auto find = Translation.find(InEntity);
    if (find != Translation.end())
        return find->second;
    return -1;
}
