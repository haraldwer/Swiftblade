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
    Physics::Manager::Get().SetVelocity(GetID(), InVelocity); 
}

void ECS::Rigidbody::ClearForces() const
{
    Physics::Manager::Get().ClearForces(GetID());
}

void ECS::Rigidbody::SetKinematic(bool InKinematic) const
{
    Physics::Manager::Get().SetKinematic(GetID(), InKinematic);
}

Vec3F ECS::Rigidbody::GetVelocity() const
{
    return Physics::Manager::Get().GetVelocity(GetID());
}

void ECS::SysRigidbody::Update(EntityID InID, Rigidbody& InComponent, double InDelta)
{
    InComponent.AddForce(Vec3F::Up() * -50.0f * InComponent.GravityScale.Get()); 
}
