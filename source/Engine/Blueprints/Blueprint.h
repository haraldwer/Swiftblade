#pragma once
#include "Engine/ECS/entity.h"
#include "Engine/Resource/Resource.h"

class BlueprintResource
{
public:

    bool Load(const String& InIdentifier);
    bool Unload();

    ECS::EntityID Instantiate();

private:

    rapidjson::Document Doc;
    
};

typedef Resource::Ref<BlueprintResource> ResBlueprint;
