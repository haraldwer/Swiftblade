#include "Engine/ECS/Manager.h"

#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "Player.h"
#include "Rigidbody.h"

using namespace ECS;

void Manager::RegisterSystems()
{
#define REG_ENTITY_SYSTEM(x) { \
auto var = new CONCAT(Sys, x) (); \
SystemMap[typeid(CONCAT(Sys, x)).hash_code()] = var; \
ComponentMap[typeid(x).hash_code()] = var; \
NameMap[String(#x)] = var; } \

    // Register systems here!
    REG_ENTITY_SYSTEM(Transform);
    REG_ENTITY_SYSTEM(Mesh);
    REG_ENTITY_SYSTEM(Camera);
    REG_ENTITY_SYSTEM(Collider);
    REG_ENTITY_SYSTEM(Rigidbody);
    REG_ENTITY_SYSTEM(Player);
    
#undef REG_OBJ_SYS
}
