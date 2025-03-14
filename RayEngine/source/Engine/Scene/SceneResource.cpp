#include "SceneResource.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "Utility/File/File.h"

void SceneInstance::Destroy()
{
    for (const ECS::EntityID id : Entities)
    {
        CHECK_ASSERT(id == ECS::InvalidID, "Entity invalid");
        ECS::Manager::Get().DestroyEntity(id);
    }
    Entities.clear();
}

SceneInstance SceneResource::Create(const Mat4F& InOffset, bool InIsRoot) const
{
    SceneInstance instance;
    if (!InIsRoot)
        if (Doc.HasMember("Data") && Doc["Data"].IsObject())
            Utility::Deserialize(Doc["Data"].GetObj(), "Offset", instance.Offset);
    instance.Offset *= InOffset;
    
    CHECK_RETURN(!Doc.HasMember("Objects"), {})
    CHECK_RETURN(!Doc["Objects"].IsArray(), {}); 
    ECS::Manager& man = ECS::Manager::Get();
    for (const auto& entry : Doc["Objects"].GetArray())
    {
        CHECK_CONTINUE(!entry.IsObject());
        const auto& obj = entry.GetObj();  
        
        Vector<DeserializeObj> overrides;
        if (obj.HasMember("Overrides") && obj["Overrides"].IsObject())
            overrides.push_back(obj["Overrides"].GetObj());  
        
        ECS::EntityID entity = ECS::InvalidID;
        
        String bp;
        Utility::Deserialize(obj, "Blueprint", bp);
        if (const auto bpRes = ResBlueprint(bp).Get())
        {
            entity = bpRes->Instantiate(instance.Offset, overrides);
            CHECK_CONTINUE_LOG(entity == ECS::InvalidID, "Invalid ID");
        }
        else
        {
            entity = man.CreateEntity();
            CHECK_CONTINUE_LOG(entity == ECS::InvalidID, "Invalid ID");
            man.Deserialize(entity, instance.Offset, overrides);
        }
        instance.Entities.insert(entity);
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
    if (!InInstance.Entities.empty())
    {
        writer.Key("Objects");
        writer.StartArray();
        auto& ecs = ECS::Manager::Get();
        for (const ECS::EntityID entity : InInstance.Entities)
        {
            writer.StartObject();
            
            // Write blueprint name
            if (const auto attr = ecs.GetComponent<ECS::Attributes>(entity))
                Utility::Serialize(writer, "Blueprint", attr->Blueprint.Identifier()); 

            // Write entity data
            writer.Key("Overrides");
            ecs.Serialize(entity, writer, false);
            
            writer.EndObject(); 
        }
        writer.EndArray();
    }
    writer.EndObject();
    
    const String formatted = Utility::FormatJson(s.GetString());
    return Utility::WriteFile(Identifier, formatted);
}

bool SceneResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    Doc = rapidjson::Document();
    if (!Utility::FileExists(InIdentifier))
    {
        Doc.Parse("{}");
        LOG("Scene file does not exist");
        return true;
    }
    const String fileContent = Utility::ReadFile(InIdentifier);
    if (fileContent.empty())
    {
        Doc.Parse("{}");
        LOG("Scene file empty")
        return true;
    }
    Doc.Parse(fileContent.c_str());
    return true;
}

bool SceneResource::Unload()
{
    Doc = rapidjson::Document();
    return true;
}

Utility::Timepoint SceneResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);
}
