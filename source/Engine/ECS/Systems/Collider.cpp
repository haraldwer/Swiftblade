#include "Collider.h"

#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"

void ECS::SysCollider::Init(EntityID InEntity, Collider& InComponent)
{
    if (Engine::InstanceBase::Get().IsEditor())
        return; 
    Physics::Manager::Get().Add(InEntity); 
}

void ECS::SysCollider::Deinit(EntityID InEntity, Collider& InComponent)
{
    if (Engine::InstanceBase::Get().IsEditor())
        return; 
    Physics::Manager::Get().Remove(InEntity); 
}
