#include "Enemy.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "GameState.h"

void ECS::SysEnemy::Tick(EntityID InID, Enemy& InComponent)
{
    auto& trans = Get<Transform>(InID);
    const EntityID player = GameState::Get().GetPlayerID();
    CHECK_RETURN(player == ECS::INVALID_ID);
    const auto& playerTrans = Get<Transform>(player);

    // Rotate towards player
    // Add force forward

    const Vec3F dir = (playerTrans.GetPosition() - trans.GetPosition()).GetNormalized(); 
    const QuatF rot = QuatF::FromDirection(dir);
    trans.SetRotation(rot);
    
    Get<Rigidbody>(InID).AddForce(dir * InComponent.MovementSpeed.Get());  
}

void ECS::SysEnemy::Spawn(EntityID InSpawnerID)
{
    const BlueprintResource* bp = ResBlueprint("Gameplay/Enemies/BP_Enemy.json").Get();
    CHECK_RETURN_LOG(!bp, "Invalid blueprint");
    const Transform* spawnTrans = TryGet<Transform>(InSpawnerID);
    CHECK_RETURN_LOG(!spawnTrans, "Spawner missing transform");
    const EntityID enemy = bp->Instantiate(spawnTrans->World()); 
    CHECK_RETURN_LOG(enemy == ECS::INVALID_ID, "Failed to instantiate enemy");
}

void ECS::SysEnemy::ApplyDamage(const EntityID InEnemy, EntityID InDamageDealer)
{
    LOG("Enemy killed!");
    Manager::Get().DestroyEntity(InEnemy); 
}
