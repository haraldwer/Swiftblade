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
    PROPERTY(String, Overrides); // This is supposed to point to the data 
};

class Scene : public PropertyOwner<Scene>
{
public:
    SceneInstance Create() const;
    bool Save(const String& InPath) const override;
    bool Load(const String& InPath) override;
    Utility::Timepoint GetEditTime() const; 
protected:
    PROPERTY(Vector<SceneObject>, Objects);
    String Identifier; 
};

typedef Resource::Ref<Scene> ResScene;