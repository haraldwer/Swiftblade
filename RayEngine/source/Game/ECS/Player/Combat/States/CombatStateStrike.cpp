#include "CombatStateStrike.h"

#include "CombatStateIdle.h"
#include "ECS/Enemies/Enemy.h"
#include "ECS/Player/Animation/States/AnimationStateStrike.h"
#include "ECS/Player/Combat/Weapon/Weapon.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"
#include "Input/Action.h"
#include "Engine/Editor/Debug/Draw.h"

Utility::Type CombatStateStrike::Update()
{
    const auto weapon = GetWeapon();
    CHECK_RETURN(!weapon, Type::Get<CombatStateIdle>());
    
    const auto& camTrans = GetCameraTransform();
    const auto world = camTrans.World();

    Physics::SweepParams params;
    params.start = world.GetPosition() + world.Forward() * 1.0f;
    params.end = params.start + world.Forward() * 0.1f;
    params.shape = Physics::Shape::SPHERE;
    params.shapeData.xyz = SweepSize.Get();
    params.ignoredEntities = { GetPlayerID() };
    params.ignoredEntities.insert(weapon->GetID());

    Engine::DebugSphere(params.start, SweepSize);
    Engine::DebugSphere(params.end, SweepSize);

    const auto result = Physics::Query::Sweep(params);
    auto& enemySys = ECS::Manager::Get().GetSystem<SysEnemy>();
    for (const auto& hit : result.hits)
        if (enemySys.Contains(hit.entity))
            enemySys.ApplyDamage(hit.entity, weapon->GetID());

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
    return Type::Get<AnimationStateStrike>();
}

