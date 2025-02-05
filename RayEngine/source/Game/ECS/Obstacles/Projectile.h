#pragma once
#include "ECS/Component.h"
#include "ECS/System.h"

struct Projectile : ECS::Component<Projectile>
{
    PROPERTY_C(float, Speed, 10.0f);
    PROPERTY_C(float, Lifetime, 1.0f);

    double Timer = 0.0f;
};

class SysProjectile : public ECS::System<Projectile>
{
public:
    void Init(ECS::EntityID InID, Projectile& InComponent) override;
    void Update(ECS::EntityID InID, Projectile& InComponent) override;
    void OnBeginContact(const Physics::Contact& InContact) override;
};
