#pragma once

#include "ECS/Entity.h"
#include "Resource/Resource.h"

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

    static bool Accept(const String& InPath);
    
protected:
    DocumentObj doc = {}; 
};

typedef Resource::Ref<SceneResource> ResScene;