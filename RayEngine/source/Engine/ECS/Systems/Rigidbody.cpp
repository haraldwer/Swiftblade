#include "Rigidbody.h"

void ECS::Rigidbody::AddForce(const Vec3F& InForce) const
{
    CHECK_RETURN(!ptr);
}

void ECS::Rigidbody::AddImpulse(const Vec3F& InForce) const
{
    CHECK_RETURN(!ptr);
}

void ECS::Rigidbody::SetVelocity(const Vec3F& InVelocity) const
{
    CHECK_RETURN(!ptr); 
}

void ECS::Rigidbody::ClearForces() const
{
    CHECK_RETURN(!ptr);
}

void ECS::Rigidbody::SetKinematic(const bool InKinematic) const
{
    CHECK_RETURN(!ptr); 
}

Vec3F ECS::Rigidbody::GetVelocity() const
{
    CHECK_RETURN(!ptr, Vec3F::Zero())
}

void ECS::SysRigidbody::Update(EntityID InID, ECS::Rigidbody& InComponent)
{
    // Gravity
    InComponent.AddForce(Vec3F::Up() * -50.0f * InComponent.GravityScale.Get()); 
}
