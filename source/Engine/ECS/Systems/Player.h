#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Player : Component
    {
        PROPERTY_P(float, MovementForce, 200.0); 
        PROPERTY_P(float, JumpForce, 50.0); 
    };

    class SysPlayer : public System<Player>
    {
    public:
        void Update(EntityID InID, Player& InComponent, double InDelta) override;
    };
}