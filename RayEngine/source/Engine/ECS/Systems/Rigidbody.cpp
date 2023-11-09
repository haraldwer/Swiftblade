#include "Rigidbody.h"

#include "Engine/Physics/Manager.h"

void ECS::Rigidbody::AddForce(const Vec3F& InForce) const
{
    Physics::Manager::Get().AddForce(GetID(), InForce, Physics::ForceMode::FORCE); 
}

void ECS::Rigidbody::AddImpulse(const Vec3F& InForce) const
{
    Physics::Manager::Get().AddForce(GetID(), InForce, Physics::ForceMode::IMPULSE); 
}

void ECS::Rigidbody::SetVelocity(const Vec3F& InVelocity) const
{
    Physics::Manager::Get().AddForce(GetID(), InVelocity, Physics::ForceMode::VELOCITY); 
}
