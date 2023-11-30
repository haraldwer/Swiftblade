#include "Attributes.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/UniqueComponent.h"

#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Game/Systems/CubeVolume.h"
#include "Game/Systems/RoomEnd.h"
#include "Game/Systems/Player/Input.h"
#include "Game/Systems/Player/PlayerCamera.h"
#include "Game/Systems/Player/Movement/Movement.h"
#include "Game/Systems/Player/Player.h"

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
    REG_ENTITY_SYSTEM(RoomEnd);
    REG_UNIQUE_COMPONENT(Player);
    REG_UNIQUE_COMPONENT(Input);
    REG_UNIQUE_COMPONENT(Movement);
    REG_UNIQUE_COMPONENT(PlayerCamera);
    
#undef REG_OBJ_SYS
}
