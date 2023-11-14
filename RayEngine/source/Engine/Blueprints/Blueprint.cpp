#include "Blueprint.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Utility/File.h"
#include "Utility/JsonUtility.h"

bool BlueprintResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    const String fileContent = Utility::ReadFile(InIdentifier);
    CHECK_RETURN_LOG(fileContent.empty(), "Blueprint file empty", false);
    Doc = rapidjson::Document();
    Doc.Parse(fileContent.c_str());
    LOG("Blueprint loaded successfully");
    return true;
}

bool BlueprintResource::Unload()
{
    Doc = rapidjson::Document();
    return true;
}

Utility::Timepoint BlueprintResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);
}

ECS::EntityID BlueprintResource::Instantiate(const DeserializeObj* InOverrideObj)
{
    CHECK_RETURN_LOG(!Doc.IsObject(), "Invalid format", false);

    // Create entity
    ECS::Manager& man = ECS::Manager::Get();
    const ECS::EntityID id = man.CreateEntity();
    CHECK_RETURN_LOG(id == ECS::InvalidID, "Invalid ID", ECS::InvalidID);
    
    // Read doc
    SysCollection systems;

    const DocumentObj& doc = Doc;
    const auto obj = doc.GetObj();
    Deserialize(id, obj, systems, 0);
    
    // Read overrides
    if (InOverrideObj && !InOverrideObj->ObjectEmpty())
        Deserialize(id, *InOverrideObj, systems, 0);

    // Systems should initialize in order of depth, reversed
    struct SysEntry
    {
        ECS::EntityID ID = ECS::InvalidID;
        ECS::SystemBase* Sys = nullptr; 
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
    
    return id;
}

void BlueprintResource::Deserialize(ECS::EntityID InID, const DeserializeObj& InObj, SysCollection& OutSystems, int InDepth)
{
    Set<ECS::SystemBase*> systems = DeserializeComponents(InID, InObj);
    auto& entry = OutSystems[InID];
    entry.Depth = InDepth;
    entry.Systems.insert(systems.begin(), systems.end());
    DeserializeChildren(InID, InObj, OutSystems, InDepth + 1);
}

Set<ECS::SystemBase*> BlueprintResource::DeserializeComponents(ECS::EntityID InID, const DeserializeObj& InObj)
{
    // Read components
    CHECK_RETURN(!InObj.HasMember("Components"), {});
    const auto& compMember = InObj["Components"];
    CHECK_RETURN(!compMember.IsArray(), {});

    ECS::Manager& man = ECS::Manager::Get();
    Set<ECS::SystemBase*> systems;
    for (const auto& comp : compMember.GetArray())
    {
        CHECK_CONTINUE_LOG(!comp.IsObject(), "Invalid component");
        CHECK_CONTINUE_LOG(!comp.HasMember("Name"), "Missing name member");
        String name = comp["Name"].GetString();
        ECS::SystemBase* sys = man.GetSystem(name);
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

void BlueprintResource::DeserializeChildren(ECS::EntityID InID, const DeserializeObj& InObj, SysCollection& OutSystems, int InDepth)
{
    // Read children
    CHECK_RETURN(!InObj.HasMember("Children"))
    const auto& childMember = InObj["Children"];
    CHECK_RETURN(!childMember.IsArray());

    ECS::Manager& man = ECS::Manager::Get();
    auto& transSys = man.GetSystem<ECS::SysTransform>();
    for (const auto& child : childMember.GetArray())
    {
        CHECK_CONTINUE_LOG(!child.IsObject(), "Child wasnt object");
        ECS::EntityID childID = childID = man.CreateEntity();

        // Read blueprint
        ResBlueprint bp; 
        Utility::Deserialize(child.GetObj(), "BP", bp);
        if (BlueprintResource* loadedBP = bp.Get())
        {
            if (loadedBP->Doc.IsObject())
            {
                const DocumentObj& doc = loadedBP->Doc;
                const auto obj = doc.GetObj();
                Deserialize(childID, obj, OutSystems, InDepth);
            }
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

void BlueprintResource::SerializeEntity(ECS::EntityID InID, SerializeObj& OutObj)
{
    OutObj.StartObject();

    // Write components
    OutObj.Key("Components");
    OutObj.StartArray();
    const ECS::Manager& man = ECS::Manager::Get();
    for (auto& sys : man.GetAllSystems())
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
    if (auto* t = man.GetComponent<ECS::Transform>(InID))
    {
        const Set<ECS::EntityID>& children = t->GetChildren();
        if (!children.empty())
        {
            OutObj.Key("Children");
            OutObj.StartArray();
            for (const ECS::EntityID child : children)
            {
                OutObj.StartObject();
                String bp;
                if (!bp.empty())
                {
                    OutObj.Key("BP");
                    OutObj.String(bp.c_str()); // TODO: Write object BP
                }
                OutObj.Key("Overrides");
                SerializeEntity(child, OutObj);
                OutObj.EndObject();
            }
            OutObj.EndArray();
        }
    }
    
    OutObj.EndObject();
}

void BlueprintResource::Serialize(ECS::EntityID InID)
{
    CHECK_RETURN_LOG(InID == ECS::InvalidID, "Invalid ID");

    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);

    SerializeEntity(InID, writer); 

    // Format
    const String formatted = Utility::FormatJson(s.GetString());

    // Set doc
    Doc = rapidjson::Document();
    Doc.Parse(formatted.c_str());
    
    // Write to file!
    Utility::WriteFile(Identifier, formatted);
}
