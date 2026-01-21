#include "EnemySpawner.h"

#include "Enemy.h"
#include "ECS/Manager.h"
#include "Instance/Instance.h"
#include "Math/Random.hpp"

void ECS::SysEnemySpawner::Update(EntityID InID, EnemySpawner &InComponent)
{
    System::Update(InID, InComponent);
    
    InComponent.cooldown -= Utility::Time::Get().Delta();
    if (InComponent.cooldown < 0.0f)
    {
        GetSystem<SysEnemy>().Spawn(InID);
        InComponent.cooldown = Utility::Math::RandomFloat(2.0f, 5.0f);
    }
}

bool ECS::SysEnemySpawner::ShouldUpdate() const
{
    return !Engine::Instance::Get().IsEditor();
}
