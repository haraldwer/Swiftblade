#include "Blueprint.h"

#include "Engine/ECS/Manager.h"
#include "Utility/File.h"

bool BlueprintResource::Load(const String& InIdentifier)
{
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
    auto& man = ECS::Manager::Get();
    const ECS::EntityID id = man.CreateEntity();

    // TODO: Maybe load metadata?

    CHECK_RETURN_LOG(!Doc.IsObject(), "Invalid format", ECS::InvalidID);
    for (auto& comp : Doc.GetObject()["Components"].GetArray())
    {
        CHECK_CONTINUE_LOG(!comp.IsObject(), "Invalid component");
        CHECK_CONTINUE_LOG(!comp.HasMember("Name"), "Missing name member");
        String name = comp["Name"].GetString();
        ECS::SystemBase* sys = man.GetSystem(name);
        CHECK_ASSERT(!sys, "Unable to find system")
        comp.HasMember("Data") ? 
            sys->Register(id, comp["Data"].GetObject()) :
            sys->Register(id);
    }

    return id;
}
