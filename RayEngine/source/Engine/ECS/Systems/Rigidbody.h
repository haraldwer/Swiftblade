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

        PROPERTY_P(bool, LockRotation, true);

        void AddForce(const Vec3F& InForce) const;
        void AddImpulse(const Vec3F& InForce) const;
        void SetVelocity(const Vec3F& InVelocity) const;
    };

    class SysRigidbody : public System<Rigidbody>
    {
    };
}
