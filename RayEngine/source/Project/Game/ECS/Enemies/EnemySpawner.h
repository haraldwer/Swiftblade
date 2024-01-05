#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct EnemySpawner : ECS::Component<EnemySpawner>
{
};

class SysEnemySpawner : public ECS::System<EnemySpawner>
{
    void Init(ECS::EntityID InID, EnemySpawner& InComponent) override;
};
