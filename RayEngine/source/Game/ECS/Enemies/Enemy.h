#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct Enemy : ECS::Component<Enemy>
{
    // Has HP
    // Can die

    // State machine?

    PROPERTY_C(float, MovementSpeed, 10.0f);
};

class SysEnemy : public ECS::System<Enemy>
{
    // Spawn enemies in waves, based on config
    // Find enemy spawner
    // When all enemies are dead, activate checkpoint

public:

    void Update(ECS::EntityID InID, Enemy& InComponent) override;
    
    void Spawn(ECS::EntityID InSpawnerID);

    static void ApplyDamage(ECS::EntityID InEnemy, ECS::EntityID InDamageDealer);
};
