#include "Manager.h"

#include <algorithm>
#include <unordered_map>

#include "System.h"
#include "Engine/Instance/Instance.h"
#include "Systems/Attributes.h"
#include "Systems/Transform.h"

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

    // Force attribute component
    auto& attr = GetSystem<SysAttributes>();
    attr.Register(id, false);
    return id;
}

void Manager::DestroyEntity(const EntityID InEntity)
{
    CHECK_ASSERT(InEntity == InvalidID, "Invalid ID");
    CHECK_ASSERT(!Entities.contains(InEntity), "Entity does not exist");
    if (const auto t = GetComponent<Transform>(InEntity))
        for (const auto child : t->GetChildren())
            DestroyEntity(child);
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

void Manager::Deserialize(EntityID InID, const Vector<DeserializeObj>& InObjects)
{
    CHECK_ASSERT(InID == InvalidID, "Invalid ID");
    
    DeserializeSysCollection systems;

    // Read objects
    for (const DeserializeObj& obj : InObjects)    
        if (!obj.ObjectEmpty())
            Deserialize(InID, obj, systems, 0);

    // Systems should initialize in order of depth, reversed
    struct SysEntry
    {
        EntityID ID = InvalidID;
        SystemBase* Sys = nullptr; 
        int Depth;
    };

    // Add all systems to one big list
    Vector<SysEntry> sysList; 
    for (const auto& entry : systems)
        for (const auto sys : entry.second.Systems)
            sysList.push_back({ entry.first, sys, entry.second.Depth});

    // Sort the list
    std::ranges::sort(sysList, [](const SysEntry& a, const SysEntry& b) {
        return a.Depth < b.Depth; // TODO: Also consider system order  
    });

    // Finish reg
    for (const auto& sys : sysList)
        if (sys.Sys) sys.Sys->FinishRegistration(sys.ID);
}

void Manager::Deserialize(EntityID InID, const DeserializeObj& InObj, DeserializeSysCollection& OutSystems, int InDepth)
{
    Set<SystemBase*> systems = DeserializeComponents(InID, InObj);
    auto& entry = OutSystems[InID];
    entry.Depth = InDepth;
    entry.Systems.insert(systems.begin(), systems.end());
    DeserializeChildren(InID, InObj, OutSystems, InDepth + 1);
}

Set<SystemBase*> Manager::DeserializeComponents(EntityID InID, const DeserializeObj& InObj)
{
    // Read components
    CHECK_RETURN(!InObj.HasMember("Components"), {});
    const auto& compMember = InObj["Components"];
    CHECK_RETURN(!compMember.IsArray(), {});

    Set<SystemBase*> systems;
    for (const auto& comp : compMember.GetArray())
    {
        CHECK_CONTINUE_LOG(!comp.IsObject(), "Invalid component");
        CHECK_CONTINUE_LOG(!comp.HasMember("Name"), "Missing name member");
        String name = comp["Name"].GetString();
        SystemBase* sys = GetSystem(name);
        CHECK_ASSERT(!sys, "Unable to find system")
        sys->Register(InID, true);
        if (comp.HasMember("Data"))
        {
            const auto& dataMember = comp["Data"];
            if (dataMember.IsObject())
                sys->Deserialize(InID, dataMember.GetObject());
        }
        systems.insert(sys);
    }
    return systems; 
}

void Manager::DeserializeChildren(EntityID InID, const DeserializeObj& InObj, DeserializeSysCollection& OutSystems, int InDepth)
{
    // Read children
    CHECK_RETURN(!InObj.HasMember("Children"))
    const auto& childMember = InObj["Children"];
    CHECK_RETURN(!childMember.IsArray());

    auto& transSys = GetSystem<ECS::SysTransform>();
    for (const auto& child : childMember.GetArray())
    {
        CHECK_CONTINUE_LOG(!child.IsObject(), "Child wasnt object");
        EntityID childID = childID = CreateEntity();

        // Read blueprint
        ResBlueprint bp; 
        Utility::Deserialize(child.GetObj(), "BP", bp);
        if (const BlueprintResource* loadedBP = bp.Get())
        {
            auto obj = loadedBP->GetObj();
            if (!obj.ObjectEmpty())
                Deserialize(childID, obj, OutSystems, InDepth);
        }

        // Apply overrides
        if (child.HasMember("Overrides"))
        {
            const auto& overrideMember = child["Overrides"];
            if (overrideMember.IsObject())
                Deserialize(childID, overrideMember.GetObj(), OutSystems, InDepth);
        }

        // Setup hierarchy
        CHECK_CONTINUE(childID == ECS::InvalidID);
        transSys.SetupHierarchy(InID, childID);
    }
}

void Manager::Serialize(const EntityID InID, SerializeObj& OutObj)
{
    CHECK_ASSERT(InID == InvalidID, "Invalid ID");
    
    OutObj.StartObject();

    // Write components
    OutObj.Key("Components");
    OutObj.StartArray();
    for (auto& sys : GetAllSystems())
    {
        CHECK_CONTINUE_LOG(!sys.second, "Sys nullptr");
        CHECK_CONTINUE(!sys.second->Contains(InID));
        OutObj.StartObject();
        OutObj.Key("Name");
        OutObj.String(sys.first.c_str());

        // Write data
        OutObj.Key("Data");
        OutObj.StartObject();
        sys.second->Serialize(InID, OutObj);
        OutObj.EndObject();
        
        OutObj.EndObject();
    }
    OutObj.EndArray();

    // Write children
    if (auto* t = GetComponent<Transform>(InID))
    {
        const Set<EntityID>& children = t->GetChildren();
        if (!children.empty())
        {
            OutObj.Key("Children");
            OutObj.StartArray();
            for (const EntityID child : children)
            {
                OutObj.StartObject();
                String bp;
                if (!bp.empty())
                {
                    OutObj.Key("BP");
                    OutObj.String(bp.c_str()); // TODO: Write object BP
                }
                OutObj.Key("Overrides");
                Serialize(child, OutObj);
                OutObj.EndObject();
            }
            OutObj.EndArray();
        }
    }
    
    OutObj.EndObject();
}
