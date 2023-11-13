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

    ECS::EntityID Instantiate(const DeserializeObj* InOverride = nullptr);
    void Serialize(ECS::EntityID InID);

private:

    struct SysTuple
    {
        Set<ECS::SystemBase*> Systems;
        int Depth = 0;
    };
    
    typedef Map<ECS::EntityID, SysTuple> SysCollection; 
    
    static void Deserialize(ECS::EntityID InID, const DeserializeObj& InObj, SysCollection& OutSystems, int InDepth); 
    static Set<ECS::SystemBase*> DeserializeComponents(ECS::EntityID InID, const DeserializeObj& InObj); 
    static void DeserializeChildren(ECS::EntityID InID, const DeserializeObj& InObj, SysCollection& OutSystems, int InDepth);

    static void SerializeEntity(ECS::EntityID InID, SerializeObj& OutObj);

    DocumentObj Doc;
    String Identifier;
    
};

typedef Resource::Ref<BlueprintResource> ResBlueprint;
