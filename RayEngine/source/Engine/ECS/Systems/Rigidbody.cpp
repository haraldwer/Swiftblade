#include "Rigidbody.h"

#include <PxRigidDynamic.h>

#include "Engine/Physics/Manager.h"
#include "Utility/PhysXUtility.h"

void ECS::Rigidbody::AddForce(const Vec3F& InForce) const
{
    CHECK_RETURN(!Ptr);
    const physx::PxVec3 force = Utility::PhysX::ConvertVec(InForce);
    Ptr->addForce(force, physx::PxForceMode::eFORCE); 
}

void ECS::Rigidbody::AddImpulse(const Vec3F& InForce) const
{
    CHECK_RETURN(!Ptr);
    const physx::PxVec3 force = Utility::PhysX::ConvertVec(InForce);
    Ptr->addForce(force, physx::PxForceMode::eIMPULSE);
}

void ECS::Rigidbody::SetVelocity(const Vec3F& InVelocity) const
{
    CHECK_RETURN(!Ptr);
    const physx::PxVec3 vel = Utility::PhysX::ConvertVec(InVelocity);
    Ptr->setLinearVelocity(vel); 
}

void ECS::Rigidbody::ClearForces() const
{
    CHECK_RETURN(!Ptr);
    Ptr->clearForce();
}

void ECS::Rigidbody::SetKinematic(bool InKinematic) const
{
    CHECK_RETURN(!Ptr);
    auto flags = Ptr->getRigidBodyFlags();
    InKinematic ?
        flags |= physx::PxRigidBodyFlag::eKINEMATIC : // Set this bit
        flags &= ~physx::PxRigidBodyFlag::eKINEMATIC; // Keep all but this bit
    Ptr->setRigidBodyFlags(flags); 
}

Vec3F ECS::Rigidbody::GetVelocity() const
{
    CHECK_RETURN(!Ptr, Vec3F::Zero())
    return Utility::PhysX::ConvertVec(Ptr->getLinearVelocity());
}

void ECS::SysRigidbody::Update(EntityID InID, Rigidbody& InComponent, double InDelta)
{
    // Gravity
    InComponent.AddForce(Vec3F::Up() * -50.0f * InComponent.GravityScale.Get()); 
}
