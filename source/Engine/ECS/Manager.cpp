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

void Manager::DeserializeEntity(EntityID InID, const DeserializeObj& InObj)
{
    for (auto& comp : InObj["Components"].GetArray())
    {
        CHECK_CONTINUE_LOG(!comp.IsObject(), "Invalid component");
        CHECK_CONTINUE_LOG(!comp.HasMember("Name"), "Missing name member");
        String name = comp["Name"].GetString();
        SystemBase* sys = GetSystem(name);
        CHECK_ASSERT(!sys, "Unable to find system")
        sys->Register(InID);
        if (comp.HasMember("Data")) 
            sys->Deserialize(InID, comp["Data"].GetObject());
    }
}

void Manager::SerializeEntity(EntityID InID, SerializeObj& OutObj) const
{
    OutObj.Key("Components");
    OutObj.StartArray();
    for (auto& sys : NameMap)
    {
        CHECK_CONTINUE(!sys.second->Contains(InID));
        OutObj.StartObject();
        OutObj.Key("Name");
        OutObj.String(sys.first.c_str());
        OutObj.Key("Data");
        OutObj.StartObject();
        sys.second->Serialize(InID, OutObj);
        OutObj.EndObject();
        OutObj.EndObject();
    }
    OutObj.EndArray();
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
