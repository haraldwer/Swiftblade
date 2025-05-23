﻿#include "MovementStateDash.h"

#include "MovementStateIdle.h"
#include "ECS/Player/PlayerInput.h"
#include "Engine/ECS/Systems/Rigidbody.h"

Type MovementStateDash::Check()
{
    const auto& input = GetInput(); 
    if (input.DashInput)
        if (GetTimeSinceExit() > Cooldown)
            if (input.MoveInput.Length() > InputDeadzone)
                return GetType();
    return Type::None(); 
}

Type MovementStateDash::Update()
{
    GetRB().SetVelocity(Direction * Speed.Get());
    if (GetTimeSinceEnter() > Duration)
        return Type::Get<MovementStateIdle>();
    return Type::None(); 
}

void MovementStateDash::Enter()
{
    MovementState::Enter(); 
    const Vec2F input = GetInput().MoveInput;
    Direction = Vec3F(input.x, UpDirTilt, input.y).GetNormalized();
}
