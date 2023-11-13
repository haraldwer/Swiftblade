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
    CHECK_RETURN_LOG(!Doc.GetObj().HasMember("Components"), "Missing component array", ECS::InvalidID);

    // Create entity
    auto& man = ECS::Manager::Get();
    const ECS::EntityID id = man.CreateEntity();
    CHECK_RETURN_LOG(id == ECS::InvalidID, "Invalid ID", ECS::InvalidID);

    Set<ECS::SystemBase*> systems; 
    const auto readObj = [&](const auto& InObj)
    {
        // Read components
        CHECK_RETURN(!InObj.HasMember("Components"));
        const auto& compMember = InObj["Components"];
        CHECK_RETURN(!compMember.IsArray());
        for (const auto& comp : compMember.GetArray())
        {
            CHECK_CONTINUE_LOG(!comp.IsObject(), "Invalid component");
            CHECK_CONTINUE_LOG(!comp.HasMember("Name"), "Missing name member");
            String name = comp["Name"].GetString();
            ECS::SystemBase* sys = man.GetSystem(name);
            CHECK_ASSERT(!sys, "Unable to find system")
            sys->Register(id, true);
            if (comp.HasMember("Data"))
            {
                const auto& dataMember = comp["Data"];
                if (dataMember.IsObject())
                    sys->Deserialize(id, dataMember.GetObject());
            }
            systems.insert(sys);
        }

        // Read children
        if (InObj.HasMember("Children"))
        {
            const auto& childMember = InObj["Children"];
            if (childMember.IsArray())
            {
                auto& transSys = man.GetSystem<ECS::SysTransform>();
                for (const auto& child : childMember.GetArray())
                {
                    CHECK_CONTINUE_LOG(!child.IsObject(), "Child wasnt object");
                    ECS::EntityID childID = ECS::InvalidID;

                    // Load BP
                    ResBlueprint bp; 
                    Utility::Deserialize(child.GetObj(), "BP", bp);
                    if (BlueprintResource* loadedBP = bp.Get())
                    {
                        if (child.HasMember("Override"))
                        {
                            const auto& overrideMember = child["Override"];
                            if (overrideMember.IsObject())
                            {
                                const DeserializeObj obj = overrideMember.GetObj();
                                childID = loadedBP->Instantiate(&obj);
                            }
                        }
                        else
                            childID = loadedBP->Instantiate();
                    }
                    
                    // In case there was no BP
                    if (childID == ECS::InvalidID) 
                        childID = man.CreateEntity();

                    // Setup hierarchy
                    CHECK_CONTINUE(childID == ECS::InvalidID);
                    transSys.SetupHierarchy(id, childID);
                }
            }
        }
    };
    
    // Read doc
    readObj(Doc.GetObject());
    
    // Read overrides
    if (InOverrideObj && !InOverrideObj->ObjectEmpty())
        readObj(*InOverrideObj);
    
    // Finish reg after all components are loaded
    for (const auto sys : systems)
        sys->FinishRegistration(id);
    
    return id;
}

void BlueprintResource::Serialize(ECS::EntityID InID)
{
    CHECK_RETURN_LOG(InID == ECS::InvalidID, "Invalid ID");

    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);

    // Write content
    writer.StartObject();
    writer.Key("Components");
    writer.StartArray();
    auto& man = ECS::Manager::Get();
    for (auto& sys : man.GetAllSystems())
    {
        CHECK_CONTINUE_LOG(!sys.second, "Sys nullptr");
        CHECK_CONTINUE(!sys.second->Contains(InID));
        writer.StartObject();
        writer.Key("Name");
        writer.String(sys.first.c_str());

        // TODO: Only write data if there are overrides
        writer.Key("Data");
        writer.StartObject();
        sys.second->Serialize(InID, writer);
        writer.EndObject();
        
        writer.EndObject();
    }
    writer.EndArray();

    // TODO: Write children, but only if they have overrides
    
    writer.EndObject();

    // Format
    const String formatted = Utility::FormatJson(s.GetString());

    // Set doc
    Doc = rapidjson::Document();
    Doc.Parse(formatted.c_str());
    
    // Write to file!
    Utility::WriteFile(Identifier, formatted);
}
