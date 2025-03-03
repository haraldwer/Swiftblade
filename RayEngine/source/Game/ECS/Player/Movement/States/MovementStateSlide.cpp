﻿#include "MovementStateSlide.h"

#include "ECS/Player/Animation/States/AnimationStateSlide.h"
#include "ECS/Player/Movement/Movement.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "MovementStateCrouch.h"
#include "MovementStateIdle.h"
#include "ECS/Player/PlayerInput.h"

Type MovementStateSlide::Check()
{
    const auto& input = GetInput();
    auto& m = GetMovement();
    if (m.IsOnGround() && input.CrouchInput && !m.IsCrouching())
    {
        if (GetRB().GetVelocity().Length() > SpeedThreshold &&
            input.MoveInput.Length() > InputDeadzone)
            return GetType(); 
        return Type::Get<MovementStateCrouch>();
    }
    return Type::None(); 
}

Type MovementStateSlide::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();
    
    movement.Look(input.RotInput);
    GetRB().SetVelocity(Direction * Speed.Get());
    
    if (!movement.IsOnGround())
    {
        LOG("Exit side because not on ground")
        return Type::Get<MovementStateIdle>();
    }
    if (!movement.IsCrouching())
    {
        LOG("Exit side because not crouching")
        return Type::Get<MovementStateIdle>();
    }
    if (!input.CrouchInput)
    {
        LOG("Exit side because does not want to crouch")
        return Type::Get<MovementStateIdle>();
    }
    if (!CheckInput())
    {
        LOG("Exit side because no input")
        return Type::Get<MovementStateIdle>();
    }
    
    return Type::None(); 
}

void MovementStateSlide::Enter()
{
    MovementState::Enter();
    GetMovement().SetCrouch(true);
    Direction = (GetRB().GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();
}

void MovementStateSlide::Exit()
{
    MovementState::Exit();
    GetMovement().SetCrouch(false);
}

Type MovementStateSlide::GetAnimationState() const
{
    return Type::Get<AnimationStateSlide>();
}

bool MovementStateSlide::CheckInput() const
{
    auto input = GetInput().MoveInput; 
    
    // Check input length
    if (input.Length() < InputDeadzone)
        return false;
    
    // Check input dot
    const Vec3F dir = Vec3F(input.x, 0.0f, input.y).GetNormalized();
    if (Vec3F::Dot(dir, Direction) < InputDot)
        return false; 

    return true; 
}
