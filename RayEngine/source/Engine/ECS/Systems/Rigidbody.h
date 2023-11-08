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
        
    };

    class SysRigidbody : public System<Rigidbody>
    {
    public:
        void AddForce(EntityID InID, const Vec3F& InForce) const;
        void AddImpulse(EntityID InID, const Vec3F& InForce) const;
        void SetVelocity(EntityID InID, const Vec3F& InVelocity) const;
    };
}
