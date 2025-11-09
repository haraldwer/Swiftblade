#pragma once

#include "ECS/Entity.h"
#include "Core/Resource/Resource.h"

namespace ECS
{
    class SystemBase;
    class Manager;
}

class BlueprintResource : public Resource::Base
{
    friend ECS::Manager;
    
public:
    bool Load() override;
    bool Unload() override;
    bool Edit(const String& InName, uint32 InOffset = 0) override;

    ECS::EntityID Instantiate(const Mat4F& InTransform = Mat4F(), const Vector<DeserializeObj>& InOverrides = {}, ECS::EntityID InID = ECS::INVALID_ID) const;
    void SaveEntity(ECS::EntityID InID);

    static bool EditAccept(const String& InPath);
    
private:
    DeserializeObj GetObj() const;
    DocumentObj doc = {};
};

typedef Resource::Ref<BlueprintResource> ResBlueprint;
