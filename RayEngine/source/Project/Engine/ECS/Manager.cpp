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
    SortSystems();
    for (SystemBase* system : SortedSystems)
        system->InitSystem();
}

void Manager::SortSystems()
{
    for (auto sys : SystemMap)
        SortedSystems.push_back(sys.second);

    std::ranges::sort(SortedSystems, [](const SystemBase* InFirst, const SystemBase* InSecond)
    {
        return InFirst->GetPriority() > InSecond->GetPriority();
    });
}

void Manager::Deinit()
{
    // Destroy entities
    for (const EntityID entity : Entities)
        DestroyEntity(entity);
    DestroyPending();

    // Delete systems
    for (const SystemBase* system : SortedSystems)
        delete(system);
    SortedSystems.clear();
    SystemMap.clear();
    ComponentMap.clear();
    NameMap.clear(); 
}

void Manager::Update(const double InDelta)
{
    for (SystemBase* system : SortedSystems)
        if (system->ShouldUpdate())
            system->UpdateSystem(InDelta);
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
    do
    {
        const Set<EntityID> copy = PendingDestroy;
        PendingDestroy.clear();
        for (EntityID obj : copy)
        {
            for (SystemBase* system : SortedSystems)
                system->Unregister(obj);
            Entities.erase(obj);
        }
    }
    while (!PendingDestroy.empty());
}

SystemBase* Manager::GetSystem(const String& InComponentName)
{
    const auto find = NameMap.find(InComponentName);
    CHECK_ASSERT(find == NameMap.end(), "Unable to find system");
    CHECK_ASSERT(!find->second, "System null");
    return find->second;
}

SystemBase* Manager::GetSystem(const Utility::Type& InType, const bool InIsCompHash)
{
    auto& map = InIsCompHash ? ComponentMap : SystemMap;
    const auto find = map.find(InType.GetHash());
    CHECK_ASSERT(find == map.end(), "Unable to find system");
    CHECK_ASSERT(!find->second, "System null");
    return find->second;
}

void Manager::Deserialize(const EntityID InID, const Mat4F& InTransform, const Vector<DeserializeObj>& InObjects)
{
    CHECK_ASSERT(InID == InvalidID, "Invalid ID");
    
    DeserializeEntityCollection systems;

    // Read objects
    for (const DeserializeObj& obj : InObjects)    
        if (!obj.ObjectEmpty())
            Deserialize(InID, obj, systems, 0);

    // Apply transformation
    for (const auto& entry : systems)
        if (auto* trans = GetComponent<Transform>(entry.first))
            if (trans->GetParent() == InvalidID) // Is root? 
                trans->SetWorld(trans->World() * InTransform);
    
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
        if (a.Depth == b.Depth)
            return a.Sys->GetPriority() > b.Sys->GetPriority();
        return a.Depth < b.Depth;
    });
    
    // Finish reg
    for (const auto& sys : sysList)
        if (sys.Sys) sys.Sys->FinishRegistration(sys.ID);
}

void Manager::Deserialize(EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, int InDepth)
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

void Manager::DeserializeChildren(EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, int InDepth)
{
    // Read children
    CHECK_RETURN(!InObj.HasMember("Children"))
    const auto& childMember = InObj["Children"];
    CHECK_RETURN(!childMember.IsArray());

    auto& transSys = GetSystem<SysTransform>();
    for (const auto& child : childMember.GetArray())
    {
        CHECK_CONTINUE_LOG(!child.IsObject(), "Child wasnt object");

        EntityID childID = InvalidID;
        
        // The child might already exist!
        // How do we identify these children? 
        
        if (childID == InvalidID)
            childID = CreateEntity();

        // Read blueprint
        ResBlueprint bp; 
        Utility::Deserialize(child.GetObj(), "BP", bp);
        if (const BlueprintResource* loadedBP = bp.Get())
        {
            auto obj = loadedBP->GetObj();
            if (!obj.ObjectEmpty())
                Deserialize(childID, obj, OutSystems, InDepth);
        }

        // TODO: Apply children overrides
        if (child.HasMember("Overrides"))
        {
            const auto& overrideMember = child["Overrides"];
            if (overrideMember.IsObject())
                Deserialize(childID, overrideMember.GetObj(), OutSystems, InDepth);
        }

        // Setup hierarchy
        // TODO: Only if child is newly created
        transSys.SetupHierarchy(InID, childID, Transform::Space::LOCAL, false);
    }
}

void Manager::Serialize(const EntityID InID, SerializeObj& OutObj, const bool InChildren)
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
        sys.second->Serialize(InID, OutObj);
        
        OutObj.EndObject();
    }
    OutObj.EndArray();

    // Write children
    if (InChildren)
    {
        if (const auto* t = GetComponent<Transform>(InID))
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
                        OutObj.String(bp.c_str()); 
                    }

                    // Serialize children overrides
                    OutObj.Key("Overrides");
                    Serialize(child, OutObj, true);
                    OutObj.EndObject();
                }
                OutObj.EndArray();
            }
        }
    }
    
    OutObj.EndObject();
}

