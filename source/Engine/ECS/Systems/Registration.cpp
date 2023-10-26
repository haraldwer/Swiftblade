#include "Engine/ECS/Manager.h"

#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Collider.h"
#include "Rigidbody.h"

using namespace ECS; 

void Manager::RegisterSystems()
{
#define REG_ENTITY_SYSTEM(x) { \
auto var = new x(); \
size_t hash = typeid(x).hash_code(); \
Systems[hash] = var; }

    // Register systems here!
    REG_ENTITY_SYSTEM(SysTransform);
    REG_ENTITY_SYSTEM(SysMesh);
    REG_ENTITY_SYSTEM(SysCamera);
    REG_ENTITY_SYSTEM(SysCollision);
    REG_ENTITY_SYSTEM(SysRigidbody);
    
#undef REG_OBJ_SYS
}
