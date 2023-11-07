#include "Blueprint.h"

#include "Engine/ECS/Manager.h"
#include "Utility/File.h"
#include "Utility/JsonUtility.h"

bool BlueprintResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    const String fileContent = Utility::ReadFile(InIdentifier);
    CHECK_RETURN_LOG(fileContent.empty(), "Prefab file empty", false);
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

ECS::EntityID BlueprintResource::Instantiate()
{
    CHECK_RETURN_LOG(!Doc.IsObject(), "Invalid format", false);
    CHECK_RETURN_LOG(!Doc.GetObj().HasMember("Components"), "Missing component array", ECS::InvalidID);

    // Create entity
    auto& man = ECS::Manager::Get();
    const ECS::EntityID id = man.CreateEntity();
    CHECK_RETURN_LOG(id == ECS::InvalidID, "Invalid ID", ECS::InvalidID);

    // Read doc
    for (auto& comp : Doc.GetObj()["Components"].GetArray())
    {
        CHECK_CONTINUE_LOG(!comp.IsObject(), "Invalid component");
        CHECK_CONTINUE_LOG(!comp.HasMember("Name"), "Missing name member");
        String name = comp["Name"].GetString();
        ECS::SystemBase* sys = man.GetSystem(name);
        CHECK_ASSERT(!sys, "Unable to find system")
        sys->Register(id);
        if (comp.HasMember("Data")) 
            sys->Deserialize(id, comp["Data"].GetObject());
    }

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
        writer.Key("Data");
        writer.StartObject();
        sys.second->Serialize(InID, writer);
        writer.EndObject();
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();

    // Format
    const String formatted = Utility::FormatJson(s.GetString());

    // Set doc
    Doc = rapidjson::Document();
    Doc.Parse(formatted.c_str());
    
    // Write to file!
    Utility::WriteFile(Identifier, formatted);
}
