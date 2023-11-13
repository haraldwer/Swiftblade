#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/Scene/Scene.h"

namespace ECS
{
    struct Player : Component<Player>
    {
        PROPERTY_P(float, MovementForce, 200.0); 
        PROPERTY_P(float, JumpForce, 50.0);
    };

    class SysPlayer : public System<Player>
    {
    public:
        void Deinit(EntityID InID, Player& InComponent) override;
        void Update(EntityID InID, Player& InComponent, double InDelta) override;
    };
}
