#pragma once

#include "Blueprints/Blueprint.h"
#include "Resource/Resource.h"

struct SceneInstance
{
    void Destroy();
    Set<ECS::EntityID> Entities;
    Mat4F Offset;
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
    
    DocumentObj Doc; 
    String Identifier;
};

typedef Resource::Ref<SceneResource> ResScene;