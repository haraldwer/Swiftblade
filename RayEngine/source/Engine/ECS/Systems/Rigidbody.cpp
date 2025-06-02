#include "Rigidbody.h"

#include <PxRigidDynamic.h>

#include "Physics/PhysXUtility.h"

void ECS::Rigidbody::AddForce(const Vec3F& InForce) const
{
    CHECK_RETURN(!ptr);
    const physx::PxVec3 force = Utility::PhysX::ConvertVec(InForce);
    ptr->addForce(force, physx::PxForceMode::eFORCE); 
}

void ECS::Rigidbody::AddImpulse(const Vec3F& InForce) const
{
    CHECK_RETURN(!ptr);
    const physx::PxVec3 force = Utility::PhysX::ConvertVec(InForce);
    ptr->addForce(force, physx::PxForceMode::eIMPULSE);
}

void ECS::Rigidbody::SetVelocity(const Vec3F& InVelocity) const
{
    CHECK_RETURN(!ptr);
    const physx::PxVec3 vel = Utility::PhysX::ConvertVec(InVelocity);
    ptr->setLinearVelocity(vel); 
}

void ECS::Rigidbody::ClearForces() const
{
    CHECK_RETURN(!ptr);
    ptr->clearForce();
}

void ECS::Rigidbody::SetKinematic(const bool InKinematic) const
{
    CHECK_RETURN(!ptr);
    auto flags = ptr->getRigidBodyFlags();
    InKinematic ?
        flags |= physx::PxRigidBodyFlag::eKINEMATIC : // Set this bit
        flags &= ~physx::PxRigidBodyFlag::eKINEMATIC; // Keep all but this bit
    ptr->setRigidBodyFlags(flags); 
}

Vec3F ECS::Rigidbody::GetVelocity() const
{
    CHECK_RETURN(!ptr, Vec3F::Zero())
    return Utility::PhysX::ConvertVec(ptr->getLinearVelocity());
}

void ECS::SysRigidbody::Update(EntityID InID, ECS::Rigidbody& InComponent)
{
    // Gravity
    InComponent.AddForce(Vec3F::Up() * -50.0f * InComponent.GravityScale.Get()); 
}
