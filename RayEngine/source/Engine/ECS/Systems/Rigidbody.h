#pragma once

#include "ECS/Component.h"
#include "ECS/System.h"

namespace Physics
{
    class Manager;
}

namespace ECS
{
    struct Rigidbody : Component<Rigidbody>
    {
        PROPERTY_C(bool, LockRotation, false);
        PROPERTY_C(float, Mass, 1.0f);
        PROPERTY_C(float, LinearDamping, 0.1f);
        PROPERTY_C(float, AngularDamping, 0.5f);
        PROPERTY_C(float, MaxLinearVelocity, 100.0f); 
        PROPERTY_C(float, MaxAngularVelocity, 100.0f);
        PROPERTY_D(float, GravityScale, 1.0f); 

        void AddForce(const Vec3F& InForce) const;
        void AddImpulse(const Vec3F& InForce) const;
        void SetVelocity(const Vec3F& InVelocity) const;
        void ClearForces() const;
        void SetKinematic(bool InKinematic) const;
        
        Vec3F GetVelocity() const;

    private:
        friend Physics::Manager;
        void* ptr = nullptr; 
    };

    class SysRigidbody : public System<Rigidbody>
    {
    public:
        void Update(EntityID InID, Rigidbody& InComponent) override;
    };
}
