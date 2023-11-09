#include "Scene.h"

#include "Engine/ECS/Manager.h"

void SceneInstance::Destroy()
{
    for (const ECS::EntityID id : Entities)
    {
        CHECK_ASSERT(id == ECS::InvalidID, "Entity invalid");
        ECS::Manager::Get().DestroyEntity(id);
    }
    Entities.clear();
}

SceneInstance Scene::Create() const
{
    SceneInstance instance;
    for (const SceneObject& obj : Objects.Get())
        if (const auto bp = obj.Blueprint.Get().Get())
            instance.Entities.push_back(bp->Instantiate());
    return instance; 
}

bool Scene::Save(const String& InPath) const
{
    // TODO: Update the overrides! Using scene instance?  
    return PropertyOwner::Save(InPath);
}
