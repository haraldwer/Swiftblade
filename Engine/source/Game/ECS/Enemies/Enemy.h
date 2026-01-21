#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct Enemy : Component<Enemy>
    {
        // Has HP
        // Can die

        // State machine?

        PROPERTY_C(float, MovementSpeed, 10.0f);
    };

    class SysEnemy : public System<Enemy>
    {
        // Spawn enemies in waves, based on config
        // Find enemy spawner
        // When all enemies are dead, activate checkpoint

    public:

        void Update(EntityID InID, Enemy& InComponent) override;
        
        void Spawn(EntityID InSpawnerID);

        static void ApplyDamage(EntityID InEnemy, EntityID InDamageDealer);
    };
}
