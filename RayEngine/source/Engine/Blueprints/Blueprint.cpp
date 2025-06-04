#include "Blueprint.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "Utility/File/File.h"

bool BlueprintResource::Load(const String& InIdentifier)
{
    identifier = InIdentifier;
    const String fileContent = Utility::ReadFile(InIdentifier);
    CHECK_RETURN_LOG(fileContent.empty(), "Blueprint file empty", false);
    doc = DocumentObj();
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid object", false);
    return true;
}

bool BlueprintResource::Unload()
{
    doc = DocumentObj();
    return true;
}

Utility::Timepoint BlueprintResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);
}

ECS::EntityID BlueprintResource::Instantiate(const Mat4F& InTransform, const Vector<DeserializeObj>& InOverrides, ECS::EntityID InID) const
{
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format", false);

    // Create entity
    ECS::Manager& man = ECS::Manager::Get();
    const ECS::EntityID id = InID == ECS::INVALID_ID ?
        man.CreateEntity() : InID; 
    CHECK_RETURN_LOG(id == ECS::INVALID_ID, "Invalid ID", ECS::INVALID_ID);

    // Set the blueprint that it's based on
    if (const auto attr = man.GetComponent<ECS::Attributes>(id))
        attr->blueprint = identifier;

    // Deserialize with overrides
    Vector<DeserializeObj> vec;
    const auto obj = GetObj();
    vec.push_back(obj);
    for (const auto& override : InOverrides)
        vec.push_back(override);
    man.Deserialize(id, InTransform, vec);
    return id;
}

void BlueprintResource::Save(const ECS::EntityID InID)
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
    Utility::WriteFile(identifier, formatted);
}

DeserializeObj BlueprintResource::GetObj() const
{
    const DocumentObj& docObj = doc;
    const auto obj = docObj.GetObj();
    return obj; 
}
