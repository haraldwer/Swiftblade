#pragma once

#include "ECS/Entity.h"
#include "Resource/Resource.h"

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

    ECS::EntityID Instantiate(const Mat4F& InTransform = Mat4F(), const Vector<DeserializeObj>& InOverrides = {}, ECS::EntityID InID = ECS::InvalidID) const;
    void Save(ECS::EntityID InID);
    
    DeserializeObj GetObj() const;

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName, uint32 InOffset = 0) { return false; };

private:

    DocumentObj Doc;
    String Identifier;
    
};

typedef Resource::Ref<BlueprintResource> ResBlueprint;
