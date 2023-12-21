#include "Enemy.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"

void SysEnemy::Update(ECS::EntityID InID, Enemy& InComponent, double InDelta)
{
    auto& trans = Get<ECS::Transform>(InID);

    // Rotate towards player
    // Add force forward

    Vec3F dir = trans.World().Forward();

    
    
    
    Get<ECS::Rigidbody>(InID).AddForce(dir * InComponent.MovementSpeed.Get());  
}

void SysEnemy::Spawn(ECS::EntityID InSpawnerID)
{
    const BlueprintResource* bp = ResBlueprint("Gameplay/Enemies/BP_Enemy.json").Get();
    CHECK_RETURN_LOG(!bp, "Invalid blueprint");
    const ECS::Transform& spawnTrans = Get<ECS::Transform>(InSpawnerID);
    const ECS::EntityID enemy = bp->Instantiate(spawnTrans.World()); 
    CHECK_RETURN_LOG(enemy == ECS::InvalidID, "Failed to instantiate enemy");
    
}
