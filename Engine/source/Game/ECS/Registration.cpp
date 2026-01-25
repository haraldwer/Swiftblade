#include "Registration.h"

#include "ECS/Abilities/BoostPad.h"
#include "ECS/Abilities/GrapplePoint.h"
#include "ECS/Enemies/Enemy.h"
#include "ECS/Enemies/EnemySpawner.h"
#include "ECS/Obstacles/Obstacle.h"
#include "ECS/Obstacles/Projectile.h"
#include "ECS/Player/Animation/PlayerAnimator.h"
#include "ECS/Player/Animation/AnimationPoser.h"
#include "ECS/Player/Combat/Combat.h"
#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/Player.h"
#include "ECS/Player/PlayerCamera.h"
#include "ECS/Player/PlayerInput.h"
#include "ECS/Player/Combat/Weapon/Weapon.h"
#include "ECS/Flow/RoomConnection.h"
#include "ECS/Flow/SectionEnd.h"
#include "ECS/Flow/GameEnd.h"
#include "ECS/Flow/Checkpoint.h"
#include "ECS/Volume/CubeVolume.h"

void ECS::RegisterGameSystems()
{
    // Game systems
    ECS_REG_ENTITY_SYSTEM(CubeVolume);
    ECS_REG_ENTITY_SYSTEM(RoomConnection);
    ECS_REG_ENTITY_SYSTEM(Checkpoint);
    ECS_REG_ENTITY_SYSTEM(SectionEnd);
    ECS_REG_ENTITY_SYSTEM(GameEnd);
    ECS_REG_ENTITY_SYSTEM(Projectile);

    ECS_REG_ENTITY_SYSTEM(Enemy);
    ECS_REG_ENTITY_SYSTEM(EnemySpawner);
    
    ECS_REG_UNIQUE_COMPONENT(Player);
    ECS_REG_UNIQUE_COMPONENT(PlayerInput);
    ECS_REG_UNIQUE_COMPONENT(Movement);
    ECS_REG_UNIQUE_COMPONENT(PlayerCamera);
    ECS_REG_UNIQUE_COMPONENT(Weapon);
    ECS_REG_UNIQUE_COMPONENT(Combat);
    ECS_REG_UNIQUE_COMPONENT(PlayerAnimator);
    ECS_REG_UNIQUE_COMPONENT(AnimationPoser);
    ECS_REG_UNIQUE_COMPONENT(Obstacle);
    ECS_REG_UNIQUE_COMPONENT(BoostPad);
    ECS_REG_UNIQUE_COMPONENT(GrapplePoint);
}