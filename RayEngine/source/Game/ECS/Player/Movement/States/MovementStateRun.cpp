﻿#include "MovementStateRun.h"

#include "MovementStateIdle.h"
#include "ECS/Player/PlayerInput.h"
#include "ECS/Player/Movement/Movement.h"

Type MovementStateRun::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    movement.Look(input.RotInput, Look);
    if (!movement.Move(input.MoveInput, Move))
        movement.Slowdown(dt, Slowdown); 
    movement.VelocityClamp(dt, VelocityClamp);
    
    if (input.MoveInput.Length() < 0.5f)
        return Type::Get<MovementStateIdle>();
    
    return Type::None(); 
}
