#pragma once

#include "Blueprints/Blueprint.h"

struct SceneInstance
{
    void Destroy();
    Set<ECS::EntityID> entities = {};
    Mat4F offset = {};

    String ToStr() const;
};

class SceneResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload(); 
    Utility::Timepoint GetEditTime() const;
    
    SceneInstance Instantiate(const Mat4F& InOffset = Mat4F(), bool InIsRoot = true) const;
    bool FromInstance(const SceneInstance& InInstance) { return FromStr(InInstance.ToStr()); }
    bool FromStr(const String& InStr);

    bool Save(const String& InPath) { CHECK_ASSERT(false, "Not implemented!"); return false; }
    bool Edit(const String& InName, uint32 InOffset = 0) { CHECK_ASSERT(false, "Not implemented!"); return false; };
    
protected:
    
    DocumentObj doc = {}; 
    String identifier = {};
};

typedef Resource::Ref<SceneResource> ResScene;