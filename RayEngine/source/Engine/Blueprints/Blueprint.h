#pragma once
#include "Engine/ECS/entity.h"
#include "Engine/Resource/Resource.h"

class BlueprintResource
{
public:

    bool Load(const String& InIdentifier);
    bool Unload();

    ECS::EntityID Instantiate();
    void Serialize(ECS::EntityID InID);

private:

    DocumentObj Doc;
    String Identifier;
    
};

typedef Resource::Ref<BlueprintResource> ResBlueprint;
