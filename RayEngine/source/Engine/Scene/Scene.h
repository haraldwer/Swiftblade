#pragma once

#include "Engine/Resource/Resource.h"
#include "Engine/Blueprints/Blueprint.h"

struct SceneInstance
{
    void Destroy();
    Set<ECS::EntityID> Entities;
    Mat4F Offset;
};

class Scene
{
public:
    SceneInstance Create(const Mat4F& InOffset = Mat4F(), bool InIsRoot = true) const;
    bool Save(const SceneInstance& InInstance, const Mat4F& InOffset) const;
    bool Load(const String& InIdentifier);
    bool Unload(); 
    Utility::Timepoint GetEditTime() const;

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName) { return false; };
    
protected:
    
    DocumentObj Doc; 
    String Identifier;
};

typedef Resource::Ref<Scene> ResScene;