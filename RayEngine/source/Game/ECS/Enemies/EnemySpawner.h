#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct EnemySpawner : Component<EnemySpawner>
    {
    };

    class SysEnemySpawner : public System<EnemySpawner>
    {
        void Init(EntityID InID, EnemySpawner& InComponent) override;
    };
}
