#pragma once

#include "Blueprints/Blueprint.h"
#include "Core/Resource/Resource.h"

struct SceneInstance
{
    void Destroy();
    Set<ECS::EntityID> entities = {};
    Mat4F offset = {};
};

class SceneResource
{
public:
    SceneInstance Create(const Mat4F& InOffset = Mat4F(), bool InIsRoot = true) const;
    bool Save(const SceneInstance& InInstance, const Mat4F& InOffset) const;
    bool Load(const String& InIdentifier);
    bool Unload(); 
    Utility::Timepoint GetEditTime() const;

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName, uint32 InOffset = 0) { return false; };
    
protected:
    
    DocumentObj doc = {}; 
    String identifier = {};
};

typedef Resource::Ref<SceneResource> ResScene;