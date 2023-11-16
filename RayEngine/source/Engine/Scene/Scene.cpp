#include "Scene.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Utility/File.h"

void SceneInstance::Destroy()
{
    for (const ECS::EntityID id : Entities)
    {
        CHECK_ASSERT(id == ECS::InvalidID, "Entity invalid");
        ECS::Manager::Get().DestroyEntity(id);
    }
    Entities.clear();
}

SceneInstance Scene::Create(const Mat4F& InOffset) const
{
    SceneInstance instance;
    if (Doc.HasMember("Objects") && Doc["Objects"].IsArray())
    {
        for (const auto& entry : Doc["Objects"].GetArray())
        {
            CHECK_CONTINUE(!entry.IsObject());
            const auto& obj = entry.GetObj();  
            
            Vector<DeserializeObj> overrides;
            if (obj.HasMember("Overrides") && obj["Overrides"].IsObject())
                overrides.push_back(obj["Overrides"].GetObj());  

            ECS::Manager& man = ECS::Manager::Get();
            ECS::EntityID entity = ECS::InvalidID;
            
            String bp;
            Utility::Deserialize(obj, "Blueprint", bp);
            if (const auto bpRes = ResBlueprint(bp).Get())
            {
                entity = bpRes->Instantiate(overrides);
                CHECK_CONTINUE_LOG(entity == ECS::InvalidID, "Invalid ID");
            }
            else
            {
                entity = man.CreateEntity();
                CHECK_CONTINUE_LOG(entity == ECS::InvalidID, "Invalid ID");
                man.Deserialize(entity, overrides);
            }
            
            instance.Entities.push_back(entity);
            
            // TODO: This is too late! Before FinishRegistration
            if (auto trans = man.GetComponent<ECS::Transform>(entity))
            {
                if (trans->GetParent() == ECS::InvalidID)
                {
                    Mat4F world = trans->World();
                    world *= InOffset;
                    trans->SetWorld(world); 
                }
            }
        }
    }
    return instance; 
}

bool Scene::Save(const SceneInstance& InInstance) const
{
    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);

    writer.StartObject();
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
            ecs.Serialize(entity, writer);
            
            writer.EndObject(); 
        }
        writer.EndArray();
    }
    writer.EndObject();
    
    const String formatted = Utility::FormatJson(s.GetString());
    return Utility::WriteFile(Identifier, formatted);
}

bool Scene::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    const String fileContent = Utility::ReadFile(InIdentifier);
    CHECK_RETURN_LOG(fileContent.empty(), "Scene file empty", false);
    Doc = rapidjson::Document();
    Doc.Parse(fileContent.c_str());
    return true;
}

bool Scene::Unload()
{
    Doc = rapidjson::Document();
    return true;
}

Utility::Timepoint Scene::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);
}
