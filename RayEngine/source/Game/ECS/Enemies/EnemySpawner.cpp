#include "EnemySpawner.h"

#include "Enemy.h"

void SysEnemySpawner::Init(ECS::EntityID InID, EnemySpawner& InComponent)
{
    System::Init(InID, InComponent);
    GetSystem<SysEnemy>().Spawn(InID);
}
