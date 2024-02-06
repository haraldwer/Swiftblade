#include "Attributes.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/UniqueComponent.h"

#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Game/ECS/Checkpoint.h"
#include "Game/ECS/Volume/CubeVolume.h"
#include "Game/ECS/SectionEnd.h"
#include "Game/ECS/GameEnd.h"
#include "Game/ECS/RoomConnection.h"
#include "Game/ECS/Enemies/Enemy.h"
#include "Game/ECS/Enemies/EnemySpawner.h"
#include "Game/ECS/Player/PlayerInput.h"
#include "Game/ECS/Player/PlayerCamera.h"
#include "Game/ECS/Player/Movement/Movement.h"
#include "Game/ECS/Player/Player.h"
#include "Game/ECS/Player/Weapon/Weapon.h"
#include "Game/ECS/Player/Combat/Combat.h"

using namespace ECS;

void Manager::RegisterSystems()
{
#define REG_ENTITY_SYSTEM(x) { \
auto var = new CONCAT(Sys, x) (); \
SystemMap[typeid(CONCAT(Sys, x)).hash_code()] = var; \
ComponentMap[typeid(x).hash_code()] = var; \
NameToSystem[String(#x)] = var; \
SystemToName[var] = String(#x); } 

#define REG_UNIQUE_COMPONENT(x) { \
auto var = new UniqueSystem<x>(); \
SystemMap[typeid(UniqueSystem<x>).hash_code()] = var; \
ComponentMap[typeid(x).hash_code()] = var; \
NameToSystem[String(#x)] = var; \
SystemToName[var] = String(#x); }

    // Register systems here!
    REG_ENTITY_SYSTEM(Attributes);
    REG_ENTITY_SYSTEM(Transform);
    REG_ENTITY_SYSTEM(Mesh);
    REG_ENTITY_SYSTEM(Camera);
    REG_ENTITY_SYSTEM(Collider);
    REG_ENTITY_SYSTEM(Rigidbody);

    // Game systems
    REG_ENTITY_SYSTEM(CubeVolume);
    REG_ENTITY_SYSTEM(VolumeDresser);
    REG_ENTITY_SYSTEM(RoomConnection);
    REG_ENTITY_SYSTEM(Checkpoint);
    REG_ENTITY_SYSTEM(SectionEnd);
    REG_ENTITY_SYSTEM(GameEnd);

    REG_ENTITY_SYSTEM(Enemy);
    REG_ENTITY_SYSTEM(EnemySpawner);
    
    REG_UNIQUE_COMPONENT(Player);
    REG_UNIQUE_COMPONENT(PlayerInput);
    REG_UNIQUE_COMPONENT(Movement);
    REG_UNIQUE_COMPONENT(PlayerCamera);
    REG_UNIQUE_COMPONENT(Weapon);
    REG_UNIQUE_COMPONENT(Combat);
    
#undef REG_ENTITY_SYSTEM
#undef REG_UNIQUE_COMPONENT
}
