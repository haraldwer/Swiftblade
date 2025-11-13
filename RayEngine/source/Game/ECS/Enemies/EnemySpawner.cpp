#include "EnemySpawner.h"

#include "Enemy.h"
#include "ECS/Manager.h"
#include "Instance/Instance.h"

void ECS::SysEnemySpawner::Init(EntityID InID, EnemySpawner& InComponent)
{
    System::Init(InID, InComponent);

    if (!Engine::Instance::Get().IsEditor())
    {
        GetSystem<SysEnemy>().Spawn(InID);
        Manager::Get().DestroyEntity(InID);
    }
}
