#include "SceneResource.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "Utility/File/File.h"

void SceneInstance::Destroy()
{
    for (const ECS::EntityID id : entities)
    {
        CHECK_ASSERT(id == ECS::InvalidID, "Entity invalid");
        ECS::Manager::Get().DestroyEntity(id);
    }
    entities.clear();
}

SceneInstance SceneResource::Create(const Mat4F& InOffset, bool InIsRoot) const
{
    SceneInstance instance;
    if (!InIsRoot)
        if (doc.HasMember("Data") && doc["Data"].IsObject())
            Utility::Deserialize(doc["Data"].GetObj(), "Offset", instance.offset);
    instance.offset *= InOffset;
    
    CHECK_RETURN(!doc.HasMember("Objects"), {})
    CHECK_RETURN(!doc["Objects"].IsArray(), {}); 
    ECS::Manager& man = ECS::Manager::Get();
    for (const auto& entry : doc["Objects"].GetArray())
    {
        CHECK_CONTINUE(!entry.IsObject());
        const auto& obj = entry.GetObj();  
        
        Vector<DeserializeObj> overrides;
        if (obj.HasMember("Overrides") && obj["Overrides"].IsObject())
            overrides.push_back(obj["Overrides"].GetObj());  
        
        ECS::EntityID entity = ECS::INVALID_ID;
        
        String bp;
        Utility::Deserialize(obj, "Blueprint", bp);
        if (const auto bpRes = ResBlueprint(bp).Get())
        {
            entity = bpRes->Instantiate(instance.offset, overrides);
            CHECK_CONTINUE_LOG(entity == ECS::INVALID_ID, "Invalid ID");
        }
        else
        {
            entity = man.CreateEntity();
            CHECK_CONTINUE_LOG(entity == ECS::INVALID_ID, "Invalid ID");
            man.Deserialize(entity, instance.offset, overrides);
        }
        instance.entities.insert(entity);
    }
    return instance; 
}

bool SceneResource::Save(const SceneInstance& InInstance, const Mat4F& InOffset) const
{
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);

    writer.StartObject();

    // Write offset
    writer.Key("Data");
    writer.StartObject();
    Utility::Serialize(writer, "Offset", InOffset);
    writer.EndObject();

    // Write entities
    if (!InInstance.entities.empty())
    {
        writer.Key("Objects");
        writer.StartArray();
        auto& ecs = ECS::Manager::Get();
        for (const ECS::EntityID entity : InInstance.entities)
        {
            writer.StartObject();
            
            // Write blueprint name
            if (const auto attr = ecs.GetComponent<ECS::Attributes>(entity))
                Utility::Serialize(writer, "Blueprint", attr->blueprint.Identifier()); 

            // Write entity data
            writer.Key("Overrides");
            ecs.Serialize(entity, writer, false);
            
            writer.EndObject(); 
        }
        writer.EndArray();
    }
    writer.EndObject();
    
    const String formatted = Utility::FormatJson(s.GetString());
    return Utility::WriteFile(identifier, formatted);
}

bool SceneResource::Load(const String& InIdentifier)
{
    identifier = InIdentifier;
    doc = rapidjson::Document();
    if (!Utility::FileExists(InIdentifier))
    {
        doc.Parse("{}");
        LOG("Scene file does not exist");
        return true;
    }
    const String fileContent = Utility::ReadFile(InIdentifier);
    if (fileContent.empty())
    {
        doc.Parse("{}");
        LOG("Scene file empty")
        return true;
    }
    doc.Parse(fileContent.c_str());
    return true;
}

bool SceneResource::Unload()
{
    doc = rapidjson::Document();
    return true;
}

Utility::Timepoint SceneResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);
}
