#include "Collider.h"

#include "Engine/Physics/Manager.h"

void ECS::SysCollider::Init(EntityID InEntity, Collider& InComponent)
{
    Physics::Manager::Get().Add(InEntity); 
}

void ECS::SysCollider::Deinit(EntityID InEntity, Collider& InComponent)
{
    Physics::Manager::Get().Remove(InEntity); 
}
