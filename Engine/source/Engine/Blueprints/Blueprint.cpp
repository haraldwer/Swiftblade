#include "Blueprint.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "../../Editor/ECS/BlueprintEditor.h"
#include "Instance/Manager.h"
#include "Utility/File/File.h"

bool BlueprintResource::Load()
{
    const String fileContent = Utility::File::Read(id.Str());
    CHECK_RETURN_LOG(fileContent.empty(), "Blueprint file empty: " + id.Str(), true);
    doc = DocumentObj();
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid object: " + id.Str(), false);
    return true;
}

bool BlueprintResource::Unload()
{
    doc = DocumentObj();
    return true;
}

ECS::EntityID BlueprintResource::Instantiate(const Mat4F& InTransform, const Vector<DeserializeObj>& InOverrides, const ECS::EntityID InID) const
{
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format", ECS::INVALID_ID);

    // Create entity
    ECS::Manager& man = ECS::Manager::Get();
    const ECS::EntityID entity = InID == ECS::INVALID_ID ?
        man.CreateEntity() : InID; 
    CHECK_RETURN_LOG(entity == ECS::INVALID_ID, "Invalid ID", ECS::INVALID_ID);

    // Set the blueprint that it's based on
    if (const auto attr = man.GetComponent<ECS::Attributes>(entity))
        attr->blueprint = id.Str();

    // Deserialize with overrides
    Vector<DeserializeObj> vec;
    vec.push_back(GetObj());
    for (const auto& override : InOverrides)
        vec.push_back(override);
    man.Deserialize(entity, InTransform, vec);
    return entity;
}

void BlueprintResource::SaveEntity(const ECS::EntityID InID)
{
    CHECK_RETURN_LOG(InID == ECS::INVALID_ID, "Invalid ID");

    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);

    ECS::Manager& man = ECS::Manager::Get();
    man.Serialize(InID, writer, true); 

    // Format
    const String formatted = Utility::FormatJson(s.GetString());

    // Set doc
    doc = rapidjson::Document();
    doc.Parse(formatted.c_str());
    
    // Write to file!
    if (Utility::File::Write(id.Str(), formatted))
        LOG(Utility::File::Name(id.Str()) + " saved")
    else LOG("Failed to save " + id.Str())
}

DeserializeObj BlueprintResource::GetObj() const
{
    const DocumentObj& docObj = doc;
    const auto obj = docObj.GetObj();
    return obj; 
}

bool BlueprintResource::Accept(const String &InPath)
{
    return InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("BP_");
}
