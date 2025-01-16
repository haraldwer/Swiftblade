#include "Combat.h"

#include "ECS/Enemies/Enemy.h"
#include "ECS/Player/Combat/Weapon/Weapon.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"

void Combat::Update()
{
    const auto sword = GetWeapon();
    CHECK_RETURN(!sword); 
    
    // Attack
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        const auto& camTrans = GetCameraTransform();
        const auto world = camTrans.World();
        
        Physics::SweepParams params;
        params.Start = world.GetPosition() + world.Forward() * 1.0f;
        params.End = params.Start + world.Forward() * 0.1f;
        params.Shape = Physics::Shape::SPHERE;
        params.ShapeData.xyz = DamageSweepSize;
        params.IgnoredEntities = { GetPlayerID() };
        if (const auto weapon = GetWeapon())
            params.IgnoredEntities.insert(weapon->GetID());

        //Rendering::DebugSphere(params.Start, DamageSweepSize, MAGENTA);
        //Rendering::DebugSphere(params.End, DamageSweepSize, MAGENTA);

        const auto result = Physics::Query::Sweep(params);
        auto& enemySys = GetSystem<SysEnemy>();
        for (const auto& hit : result.Hits)
            if (enemySys.Contains(hit.Entity))
                enemySys.ApplyDamage(hit.Entity, GetID()); 
    }
}
