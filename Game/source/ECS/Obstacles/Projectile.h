#pragma once
#include "ECS/Component.h"
#include "ECS/System.h"

namespace ECS
{
    struct Projectile : ECS::Component<Projectile>
    {
        PROPERTY_C(float, Speed, 10.0f);
        PROPERTY_C(float, Lifetime, 1.0f);

        double timer = 0.0f;
    };

    class SysProjectile : public ECS::System<Projectile>
    {
    public:
        void Init(EntityID InID, Projectile& InComponent) override;
        void Tick(EntityID InID, Projectile& InComponent) override;
    };
}
