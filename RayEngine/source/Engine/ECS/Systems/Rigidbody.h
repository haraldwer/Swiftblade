#pragma once

#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct Rigidbody : Component<Rigidbody>
    {
        // Some exposed properties
        // And pointer to physx object

        // Set velocity?
        // Add force?
        // Add impulse?

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
    };

    class SysRigidbody : public System<Rigidbody>
    {
    public:
        void Update(EntityID InID, Rigidbody& InComponent, double InDelta) override;
    };
}
