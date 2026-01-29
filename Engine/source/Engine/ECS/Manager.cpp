#include "Manager.h"

#include <algorithm>
#include <unordered_map>

#include "System.h"
#include "Systems/Attributes.h"
#include "Systems/Transform.h"

using namespace ECS; 

void Manager::Init()
{
    CHECK_ASSERT(systemMap.empty(), "No systems registered!")
    SortSystems();
    for (SystemBase* system : sortedSystems)
        system->SystemInit();
}

void Manager::SortSystems()
{
    for (auto sys : systemMap)
        sortedSystems.push_back(sys.second);

    std::ranges::sort(sortedSystems, [](const SystemBase* InFirst, const SystemBase* InSecond)
    {
        return InFirst->GetPriority() > InSecond->GetPriority();
    });
}

void Manager::Deinit()
{
    // Destroy entities
    for (const EntityID entity : entities)
        DestroyEntity(entity);
    DestroyPending();

    // Delete systems
    for (const SystemBase* system : sortedSystems)
        delete(system);
    sortedSystems.clear();
    systemMap.clear();
    componentMap.clear();
    nameToSystem.clear(); 
}

void Manager::Update()
{
    PROFILE();
    for (SystemBase* system : sortedSystems)
        if (system->ShouldTick())
            system->SystemTick();
    DestroyPending();
}

void Manager::Frame()
{
    PROFILE();
    for (SystemBase* system : sortedSystems)
        if (system->ShouldTick())
            system->SystemFrame();
    DestroyPending();
}

EntityID Manager::CreateEntity()
{
    const EntityID id = idCounter;
    entities.insert(id); 
    idCounter++;

    // Force attribute component
    auto& attr = GetSystem<SysAttributes>();
    attr.Register(id, false);
    return id;
}

void Manager::DestroyEntity(const EntityID InEntity)
{
    CHECK_ASSERT(InEntity == INVALID_ID, "Invalid ID");
    CHECK_ASSERT(!entities.contains(InEntity), "Entity does not exist");
    if (const auto t = GetComponent<Transform>(InEntity))
        for (const auto child : t->GetChildren())
            DestroyEntity(child);
    pendingDestroy.insert(InEntity); 
}

void Manager::DestroyPending()
{
    PROFILE();
    do
    {
        const Set<EntityID> copy = pendingDestroy;
        pendingDestroy.clear();
        for (EntityID obj : copy)
        {
            for (SystemBase* system : sortedSystems)
                system->Unregister(obj);
            entities.erase(obj);
        }
    }
    while (!pendingDestroy.empty());
}

SystemBase* Manager::GetSystem(const String& InComponentName)
{
    const auto find = nameToSystem.find(InComponentName);
    CHECK_ASSERT(find == nameToSystem.end(), "Unable to find system");
    CHECK_ASSERT(!find->second, "System null");
    return find->second;
}

SystemBase* Manager::GetSystem(const Utility::Type& InType, const bool InIsCompHash)
{
    auto& map = InIsCompHash ? componentMap : systemMap;
    const auto find = map.find(InType.GetHash());
    CHECK_ASSERT(find == map.end(), "Unable to find system");
    CHECK_ASSERT(!find->second, "System null");
    return find->second;
}

void Manager::Deserialize(const EntityID InID, const Mat4F& InTransform, const Vector<DeserializeObj>& InObjects)
{
    CHECK_ASSERT(InID == INVALID_ID, "Invalid ID");
    
    DeserializeEntityCollection systems;

    // Read objects
    for (const DeserializeObj& obj : InObjects)    
        if (!obj.ObjectEmpty())
            Deserialize(InID, obj, systems, 0);

    // Apply transformation
    for (const auto& entry : systems)
        if (auto* trans = GetComponent<Transform>(entry.first))
            if (trans->GetParent() == INVALID_ID) // Is root? 
                trans->SetWorld(trans->World() * InTransform);
    
    // Systems should initialize in order of depth, reversed
    struct SysEntry
    {
        EntityID id = INVALID_ID;
        SystemBase* sys = nullptr; 
        int depth = 0;
    };

    // Add all systems to one big list
    Vector<SysEntry> sysList; 
    for (const auto& entry : systems)
        for (const auto sys : entry.second.systems)
            sysList.push_back({ entry.first, sys, entry.second.depth});

    // Sort the list
    std::ranges::sort(sysList, [](const SysEntry& InA, const SysEntry& InB) {
        if (InA.depth == InB.depth)
            return InA.sys->GetPriority() > InB.sys->GetPriority();
        return InA.depth < InB.depth;
    });
    
    // Finish reg
    for (const auto& sys : sysList)
        if (sys.sys) sys.sys->FinishRegistration(sys.id);
}

void Manager::Deserialize(const EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, const int InDepth)
{
    Set<SystemBase*> systems = DeserializeComponents(InID, InObj);
    auto& entry = OutSystems[InID];
    entry.depth = InDepth;
    entry.systems.insert(systems.begin(), systems.end());
    DeserializeChildren(InID, InObj, OutSystems, InDepth + 1);
}

Set<SystemBase*> Manager::DeserializeComponents(const EntityID InID, const DeserializeObj& InObj)
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

void Manager::DeserializeChildren(const EntityID InID, const DeserializeObj& InObj, DeserializeEntityCollection& OutSystems, const int InDepth)
{
    // Read children
    CHECK_RETURN(!InObj.HasMember("Children"))
    const auto& childMember = InObj["Children"];
    CHECK_RETURN(!childMember.IsArray());

    auto& transSys = GetSystem<SysTransform>();
    for (const auto& child : childMember.GetArray())
    {
        CHECK_CONTINUE_LOG(!child.IsObject(), "Child wasnt object");

        EntityID childID = INVALID_ID;
        
        // The child might already exist!
        // How do we identify these children? 
        
        if (childID == INVALID_ID)
            childID = CreateEntity();

        // Read blueprint
        ResBlueprint bp; 
        Utility::Deserialize(child.GetObj(), "BP", bp);
        if (const BlueprintResource* bpRes = bp.Get())
        {
            auto obj = bpRes->GetObj();
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

void Manager::RegisterSystem(SystemBase *InSys, const String &InName, Utility::TypeHash InComponentHash, Utility::TypeHash InSystemHash)
{
    CHECK_ASSERT(!InSys, "System nullptr");
    CHECK_ASSERT(InName.empty(), "Invalid system name");
    CHECK_ASSERT(InComponentHash == Utility::TypeHash(), "Invalid component type");
    CHECK_ASSERT(InSystemHash == Utility::TypeHash(), "Invalid system type");
    CHECK_ASSERT(systemMap.contains(InSystemHash), "System already registered");
    CHECK_ASSERT(componentMap.contains(InComponentHash), "Component already registered");
    CHECK_ASSERT(nameToSystem.contains(InName), "System already registered");
    
    systemMap[InSystemHash] = InSys;
    componentMap[InComponentHash] = InSys;
    nameToSystem[InName] = InSys;
    systemToName[InSys] = InName; 
}

void Manager::Serialize(const EntityID InID, SerializeObj& OutObj, const bool InChildren)
{
    CHECK_ASSERT(InID == INVALID_ID, "Invalid ID");
    
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

