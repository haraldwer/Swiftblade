#pragma once

#include "Engine/Resource/Resource.h"
#include "..\Property\Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Blueprints/Blueprint.h"

struct SceneInstance
{
    void Destroy();
    Vector<ECS::EntityID> Entities;
};

class Scene
{
public:
    SceneInstance Create(const Mat4F& InOffset = Mat4F()) const;
    bool Save(const SceneInstance& InInstance) const;
    bool Load(const String& InIdentifier);
    bool Unload(); 
    Utility::Timepoint GetEditTime() const;
    
protected:
    
    DocumentObj Doc; 
    String Identifier;
};

typedef Resource::Ref<Scene> ResScene;