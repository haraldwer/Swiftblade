#include "CombatStateStrike.h"

#include "CombatStateIdle.h"
#include "ECS/Enemies/Enemy.h"
#include "ECS/Player/Combat/Weapon/Weapon.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"
#include "Input/Action.h"
#include "Rendering/Debug/Draw.h"

Utility::Type CombatStateStrike::Update()
{
    const auto weapon = GetWeapon();
    CHECK_RETURN(!weapon, Type::Get<CombatStateIdle>());
    
    const auto& camTrans = GetCameraTransform();
    const auto world = camTrans.World();

    Physics::SweepParams params;
    params.Start = world.GetPosition() + world.Forward() * 1.0f;
    params.End = params.Start + world.Forward() * 0.1f;
    params.Shape = Physics::Shape::SPHERE;
    params.ShapeData.xyz = SweepSize.Get();
    params.IgnoredEntities = { GetPlayerID() };
    params.IgnoredEntities.insert(weapon->GetID());

    Rendering::DebugSphere(params.Start, SweepSize, MAGENTA);
    Rendering::DebugSphere(params.End, SweepSize, MAGENTA);

    const auto result = Physics::Query::Sweep(params);
    auto& enemySys = ECS::Manager::Get().GetSystem<SysEnemy>();
    for (const auto& hit : result.Hits)
        if (enemySys.Contains(hit.Entity))
            enemySys.ApplyDamage(hit.Entity, weapon->GetID());

    if (GetTimeSinceEnter() > StrikeDuration)
        return Type::Get<CombatStateIdle>();
    
    return {};
}

Utility::Type CombatStateStrike::Check()
{
    if (Input::Action::Get("Strike").Pressed())
        return GetType();
    return {};
}

Type CombatStateStrike::GetAnimationState() const
{
    return {}; // Strike anim
}

