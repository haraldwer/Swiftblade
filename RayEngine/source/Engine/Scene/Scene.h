#pragma once

#include "Engine/Resource/Resource.h"
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Blueprints/Blueprint.h"

struct SceneInstance
{
    void Destroy(); 
    Vector<ECS::EntityID> Entities; 
};

struct SceneObject : PropertyOwner<SceneObject>
{
    PROPERTY(ResBlueprint, Blueprint);
    PROPERTY(String, Overrides); 
};

class Scene : public PropertyOwner<Scene>
{
public:
    SceneInstance Create() const;
    bool Save(const String& InPath) const override;
protected:
    PROPERTY(Vector<SceneObject>, Objects);
};

typedef Resource::Ref<Scene> ResScene;