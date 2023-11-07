#include "Manager.h"

#include <unordered_map>

#include "System.h"
#include "Engine/Instance/Instance.h"

using namespace ECS; 

void Manager::Init()
{
    RegisterSystems();
    for (const auto& comp : SystemMap)
        comp.second->InitSystem();
}

void Manager::Deinit()
{
    for (const EntityID entity : Entities)
        DestroyEntity(entity);
    DestroyPending();
}

void Manager::Update(const double InDelta)
{
    for (const auto& comp : SystemMap)
        if (comp.second->ShouldUpdate())
            comp.second->UpdateSystem(InDelta);
    DestroyPending();
}

EntityID Manager::CreateEntity()
{
    const EntityID id = IDCounter;
    Entities.insert(id); 
    IDCounter++;
    return id;
}

void Manager::DestroyEntity(const EntityID InEntity)
{
    PendingDestroy.insert(InEntity); 
}

void Manager::DestroyPending()
{
    const Set<EntityID> copy = PendingDestroy;
    for (EntityID obj : copy)
    {
        for (const auto& comp : SystemMap)
            comp.second->Unregister(obj);
        Entities.erase(obj);
    }
    PendingDestroy.clear();
}

SystemBase* Manager::GetSystem(const String& InComponentName)
{
    const auto find = NameMap.find(InComponentName);
    CHECK_ASSERT(find == NameMap.end(), "Unable to find system");
    CHECK_ASSERT(!find->second, "System null");
    return find->second;
}

SystemBase* Manager::GetSystem(const size_t InHash, const bool InIsCompHash)
{
    auto& map = InIsCompHash ? ComponentMap : SystemMap;
    const auto find = map.find(InHash);
    CHECK_ASSERT(find == map.end(), "Unable to find system");
    CHECK_ASSERT(!find->second, "System null");
    return find->second;
}
