#include "Collider.h"

#include "Engine/Physics/Manager.h"

void ECS::SysCollision::Init(EntityID InEntity, Collider& InComponent)
{
    Physics::Manager::Get().Add(InEntity); 
}

void ECS::SysCollision::Deinit(EntityID InEntity, Collider& InComponent)
{
    Physics::Manager::Get().Remove(InEntity); 
}
