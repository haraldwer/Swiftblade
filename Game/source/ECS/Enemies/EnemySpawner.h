#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct EnemySpawner : Component<EnemySpawner>
    {
        float cooldown = 0.0f;
    };

    class SysEnemySpawner : public System<EnemySpawner>
    {
        void Tick(EntityID InID, EnemySpawner &InComponent) override;
        bool ShouldTick() const override;
    };
}
