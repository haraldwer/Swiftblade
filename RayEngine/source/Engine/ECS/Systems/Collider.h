#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"
#include "Engine/Physics/Resources/Material.h"

namespace ECS
{
    struct Collider : Component<Collider>
    {
        // Some exposed properties
        // And pointer to physx object
        
        PROPERTY_P(ResPM, Material, "Defaults/PM_Default.json");
        
    };

    class SysCollider : public System<Collider>
    {
    public:
        
        void Init(EntityID InEntity, Collider& InComponent) override;
        void Deinit(EntityID InEntity, Collider& InComponent) override;
    };
}
