#include "Blueprint.h"

#include "Engine/ECS/Manager.h"
#include "Utility/File.h"

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

    // Create entity
    auto& man = ECS::Manager::Get();
    const ECS::EntityID id = man.CreateEntity();

    // Read doc
    CHECK_RETURN_LOG(id == ECS::InvalidID, "Invalid ID", ECS::InvalidID);
    ECS::Manager::Get().DeserializeEntity(id, Doc.GetObj());

    return id;
}

void BlueprintResource::Serialize(ECS::EntityID InID)
{
    CHECK_RETURN_LOG(InID == ECS::InvalidID, "Invalid ID");

    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    writer.StartObject();
    ECS::Manager::Get().SerializeEntity(InID, writer);
    writer.EndObject();

    // Format
    const String formatted = Utility::FormatJson(s.GetString());

    // Set doc
    Doc = rapidjson::Document();
    Doc.Parse(formatted.c_str());
    
    // Write to file!
    Utility::WriteFile(Identifier, formatted);
}
