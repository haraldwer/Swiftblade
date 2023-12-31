#include "Enemy.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/GameState.h"

void SysEnemy::Update(ECS::EntityID InID, Enemy& InComponent, double InDelta)
{
    auto& trans = Get<ECS::Transform>(InID);
    const ECS::EntityID player = GameState::Get().GetPlayerID();
    CHECK_RETURN(player == ECS::InvalidID); 
    auto& playerTrans = Get<ECS::Transform>(player);

    // Rotate towards player
    // Add force forward

    const Vec3F dir = (playerTrans.GetPosition() - trans.GetPosition()).GetNormalized(); 
    const QuatF rot = QuatF::FromDirection(dir);
    trans.SetRotation(rot);
    
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

void SysEnemy::ApplyDamage(ECS::EntityID InEnemy, ECS::EntityID InDamageDealer)
{
    LOG("Enemy killed!");
    ECS::Manager::Get().DestroyEntity(InEnemy); 
}
