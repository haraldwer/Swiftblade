#include "Attributes.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/UniqueComponent.h"

#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Game/ECS/Checkpoint.h"
#include "Game/ECS/CubeVolume.h"
#include "Game/ECS/SectionEnd.h"
#include "Game/ECS/GameEnd.h"
#include "Game/ECS/RoomConnection.h"
#include "Game/ECS/Player/Input.h"
#include "Game/ECS/Player/PlayerCamera.h"
#include "Game/ECS/Player/Movement/Movement.h"
#include "Game/ECS/Player/Player.h"
#include "Game/ECS/Player/Sword/Sword.h"

using namespace ECS;

void Manager::RegisterSystems()
{
#define REG_ENTITY_SYSTEM(x) { \
auto var = new CONCAT(Sys, x) (); \
SystemMap[typeid(CONCAT(Sys, x)).hash_code()] = var; \
ComponentMap[typeid(x).hash_code()] = var; \
NameMap[String(#x)] = var; } \

#define REG_UNIQUE_COMPONENT(x) { \
auto var = new UniqueSystem<x>(); \
SystemMap[typeid(UniqueSystem<x>).hash_code()] = var; \
ComponentMap[typeid(x).hash_code()] = var; \
NameMap[String(#x)] = var; } \

    // Register systems here!
    REG_ENTITY_SYSTEM(Attributes);
    REG_ENTITY_SYSTEM(Transform);
    REG_ENTITY_SYSTEM(Mesh);
    REG_ENTITY_SYSTEM(Camera);
    REG_ENTITY_SYSTEM(Collider);
    REG_ENTITY_SYSTEM(Rigidbody);

    // Game systems
    REG_ENTITY_SYSTEM(CubeVolume);
    REG_ENTITY_SYSTEM(RoomConnection);
    REG_ENTITY_SYSTEM(Checkpoint);
    REG_ENTITY_SYSTEM(SectionEnd);
    REG_ENTITY_SYSTEM(GameEnd);
    REG_UNIQUE_COMPONENT(Player);
    REG_UNIQUE_COMPONENT(Input);
    REG_UNIQUE_COMPONENT(Movement);
    REG_UNIQUE_COMPONENT(PlayerCamera);
    REG_UNIQUE_COMPONENT(Sword);
    
#undef REG_ENTITY_SYSTEM
#undef REG_UNIQUE_COMPONENT
}
