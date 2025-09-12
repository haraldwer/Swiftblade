#pragma once

#include "Blueprints/Blueprint.h"

struct SceneInstance
{
    void Destroy();
    Set<ECS::EntityID> entities = {};
    Mat4F offset = {};

    String ToStr(bool InFormat) const;
};

class SceneResource : Resource::Base
{
    CLASS_INFO(SceneResource, Resource::Base)
public:
    bool Load() override;
    bool Unload() override; 
    
    SceneInstance Instantiate(const Mat4F& InOffset = Mat4F(), bool InIsRoot = true) const;
    bool FromInstance(const SceneInstance& InInstance) { return FromStr(InInstance.ToStr(false)); }
    bool FromStr(const String& InStr);
    
protected:
    DocumentObj doc = {}; 
};

typedef Resource::Ref<SceneResource> ResScene;