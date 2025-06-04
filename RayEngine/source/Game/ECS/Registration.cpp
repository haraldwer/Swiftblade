
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/UniqueComponent.h"

#include "ECS/Abilities/BoostPad.h"
#include "ECS/Abilities/GrapplePoint.h"
#include "ECS/Checkpoint.h"
#include "ECS/Enemies/Enemy.h"
#include "ECS/Enemies/EnemySpawner.h"
#include "ECS/GameEnd.h"
#include "ECS/Obstacles/Obstacle.h"
#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Animation/AnimationPoser.h"
#include "ECS/Player/Combat/Combat.h"
#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/Player.h"
#include "ECS/Player/PlayerCamera.h"
#include "ECS/Player/PlayerInput.h"
#include "ECS/Player/Combat/Weapon/Weapon.h"
#include "ECS/RoomConnection.h"
#include "ECS/SectionEnd.h"
#include "ECS/Volume/CubeVolume.h"

#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/CameraComponent.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Mesh.h"
#include "Engine/ECS/Systems/Environment.h"
#include "Engine/ECS/Systems/Light.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Obstacles/Projectile.h"

using namespace ECS;

void Manager::RegisterSystems()
{
#define REG_ENTITY_SYSTEM(x) { \
auto var = new CONCAT(Sys, x) (); \
systemMap[typeid(CONCAT(Sys, x)).hash_code()] = var; \
componentMap[typeid(x).hash_code()] = var; \
nameToSystem[String(#x)] = var; \
systemToName[var] = String(#x); } 

#define REG_UNIQUE_COMPONENT(x) { \
auto var = new UniqueSystem<x>(); \
systemMap[typeid(UniqueSystem<x>).hash_code()] = var; \
componentMap[typeid(x).hash_code()] = var; \
nameToSystem[String(#x)] = var; \
systemToName[var] = String(#x); }

    // Register systems here!
    REG_ENTITY_SYSTEM(Attributes);
    REG_ENTITY_SYSTEM(Transform);
    REG_ENTITY_SYSTEM(Mesh);
    REG_ENTITY_SYSTEM(Environment);
    REG_ENTITY_SYSTEM(Light);
    REG_ENTITY_SYSTEM(CameraComponent);
    REG_ENTITY_SYSTEM(Collider);
    REG_ENTITY_SYSTEM(Rigidbody);

    // Game systems
    REG_ENTITY_SYSTEM(CubeVolume);
    REG_ENTITY_SYSTEM(RoomConnection);
    REG_ENTITY_SYSTEM(Checkpoint);
    REG_ENTITY_SYSTEM(SectionEnd);
    REG_ENTITY_SYSTEM(GameEnd);
    REG_ENTITY_SYSTEM(Projectile);

    REG_ENTITY_SYSTEM(Enemy);
    REG_ENTITY_SYSTEM(EnemySpawner);
    
    REG_UNIQUE_COMPONENT(Player);
    REG_UNIQUE_COMPONENT(PlayerInput);
    REG_UNIQUE_COMPONENT(Movement);
    REG_UNIQUE_COMPONENT(PlayerCamera);
    REG_UNIQUE_COMPONENT(Weapon);
    REG_UNIQUE_COMPONENT(Combat);
    REG_UNIQUE_COMPONENT(Animator);
    REG_UNIQUE_COMPONENT(AnimationPoser);
    REG_UNIQUE_COMPONENT(Obstacle);
    REG_UNIQUE_COMPONENT(BoostPad);
    REG_UNIQUE_COMPONENT(GrapplePoint);
    
#undef REG_ENTITY_SYSTEM
#undef REG_UNIQUE_COMPONENT
}
