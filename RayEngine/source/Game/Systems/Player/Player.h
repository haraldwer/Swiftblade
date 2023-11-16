#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Player : Component<Player>
    {
        EntityID Collider;
        EntityID Camera; 
    };

    class SysPlayer : public System<Player>
    {
    public:
        void Init(EntityID InID, Player& InComponent) override;
        void OnBeginContact(const Physics::Contact& InContact) override;
        void OnEndContact(const Physics::Contact& InContact) override;
    };
}
