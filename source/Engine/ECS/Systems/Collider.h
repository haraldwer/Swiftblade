#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct Collider : Component
    {
        // Some exposed properties
        // And pointer to physx object

        
    };

    class SysCollider : public System<Collider>
    {
    public:
        
        void Init(EntityID InEntity, Collider& InComponent) override;
        void Deinit(EntityID InEntity, Collider& InComponent) override;
    };
}
