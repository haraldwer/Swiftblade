#include "Rigidbody.h"

#include "Engine/Physics/Manager.h"

void ECS::SysRigidbody::AddForce(EntityID InID, const Vec3F& InForce) const
{
    Physics::Manager::Get().AddForce(InID, InForce, Physics::ForceMode::FORCE); 
}

void ECS::SysRigidbody::AddImpulse(EntityID InID, const Vec3F& InForce) const
{
    Physics::Manager::Get().AddForce(InID, InForce, Physics::ForceMode::IMPULSE); 
}

void ECS::SysRigidbody::SetVelocity(EntityID InID, const Vec3F& InVelocity) const
{
    Physics::Manager::Get().AddForce(InID, InVelocity, Physics::ForceMode::VELOCITY); 
}
