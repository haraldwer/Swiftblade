#include "Blueprint.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Attributes.h"
#include "Utility/File.h"
#include "Utility/JsonUtility.h"

bool BlueprintResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    const String fileContent = Utility::ReadFile(InIdentifier);
    CHECK_RETURN_LOG(fileContent.empty(), "Blueprint file empty", false);
    Doc = rapidjson::Document();
    Doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!Doc.IsObject(), "Invalid object", false);
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

ECS::EntityID BlueprintResource::Instantiate(const Mat4F& InTransform, const Vector<DeserializeObj>& InOverrides) const
{
    CHECK_RETURN_LOG(!Doc.IsObject(), "Invalid format", false);

    // Create entity
    ECS::Manager& man = ECS::Manager::Get();
    const ECS::EntityID id = man.CreateEntity();
    CHECK_RETURN_LOG(id == ECS::InvalidID, "Invalid ID", ECS::InvalidID);

    // Set the blueprint that it's based on
    if (const auto attr = man.GetComponent<ECS::Attributes>(id))
        attr->Blueprint = Identifier;

    // Deserialize with overrides
    Vector<DeserializeObj> vec;
    const auto obj = GetObj();
    vec.push_back(obj);
    for (const auto& override : InOverrides)
        vec.push_back(override);
    man.Deserialize(id, InTransform, vec);
    return id;
}

void BlueprintResource::Save(ECS::EntityID InID)
{
    CHECK_RETURN_LOG(InID == ECS::InvalidID, "Invalid ID");

    //  Json writer
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);

    ECS::Manager& man = ECS::Manager::Get();
    man.Serialize(InID, writer); 

    // Format
    const String formatted = Utility::FormatJson(s.GetString());

    // Set doc
    Doc = rapidjson::Document();
    Doc.Parse(formatted.c_str());
    
    // Write to file!
    Utility::WriteFile(Identifier, formatted);
}

DeserializeObj BlueprintResource::GetObj() const
{
    const DocumentObj& doc = Doc;
    const auto obj = doc.GetObj();
    return obj; 
}
