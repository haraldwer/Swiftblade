#include "Manager.h"

#include <unordered_map>

#include "System.h"

using namespace ECS; 

void Manager::Init()
{
    RegisterSystems();
    for (const auto& comp : Systems)
        if (const auto ptr = comp.second.Get())
            ptr->InitSystem(this);
}

void Manager::Deinit()
{
    for (const EntityID entity : Entities)
        DestroyEntity(entity);
    DestroyPending();
}

void Manager::Update(const double InDelta)
{
    for (const auto& comp : Systems)
        if (const auto ptr = comp.second.Get())
            ptr->UpdateSystem(InDelta);
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
        for (const auto& comp : Systems)
            if (auto ptr = comp.second.Get())
                ptr->Unregister(obj);
        Entities.erase(obj);
    }
    PendingDestroy.clear();
}

