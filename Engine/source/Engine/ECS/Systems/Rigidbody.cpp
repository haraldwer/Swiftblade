#include "Rigidbody.h"

#include "Physics/Utility.h"
#include "reactphysics3d/body/RigidBody.h"
#include "reactphysics3d/components/RigidBodyComponents.h"

void ECS::Rigidbody::AddForce(const Vec3F& InForce) const
{
    CHECK_RETURN(!ptr);
    ptr->applyWorldForceAtCenterOfMass(Physics::GetVec(InForce)); 
}

void ECS::Rigidbody::AddImpulse(const Vec3F& InForce) const
{
    CHECK_RETURN(!ptr);
    Vec3F vel = Physics::GetVec(ptr->getLinearVelocity());
    vel += InForce / Mass.Get();
    ptr->setLinearVelocity(Physics::GetVec(vel));
}

void ECS::Rigidbody::SetVelocity(const Vec3F& InVelocity) const
{
    CHECK_RETURN(!ptr);
    ptr->setLinearVelocity(Physics::GetVec(InVelocity));
}

void ECS::Rigidbody::ClearForces() const
{
    CHECK_RETURN(!ptr);
}

void ECS::Rigidbody::SetKinematic(const bool InKinematic) const
{
    CHECK_RETURN(!ptr);
    ptr->setType(InKinematic ? reactphysics3d::BodyType::KINEMATIC : reactphysics3d::BodyType::DYNAMIC);
}

Vec3F ECS::Rigidbody::GetVelocity() const
{
    CHECK_RETURN(!ptr, Vec3F::Zero())
    return Physics::GetVec(ptr->getLinearVelocity());
}

void ECS::SysRigidbody::Tick(EntityID InID, Rigidbody& InComponent)
{
    InComponent.AddForce(Vec3F::Up() * -50.0f * InComponent.GravityScale.Get()); 
}
