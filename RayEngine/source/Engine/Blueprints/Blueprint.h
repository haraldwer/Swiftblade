#pragma once

#include "Engine/ECS/entity.h"
#include "Engine/Resource/Resource.h"

namespace ECS
{
    class SystemBase;
}

class BlueprintResource
{
public:

    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const;

    ECS::EntityID Instantiate(const Mat4F& InTransform = Mat4F(), const Vector<DeserializeObj>& InOverrides = {}) const;
    void Save(ECS::EntityID InID);
    
    DeserializeObj GetObj() const;

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName) { return false; };

private:

    DocumentObj Doc;
    String Identifier;
    
};

typedef Resource::Ref<BlueprintResource> ResBlueprint;
