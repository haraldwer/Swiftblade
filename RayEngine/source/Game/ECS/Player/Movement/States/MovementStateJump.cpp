#include "MovementStateJump.h"

#include "MovementStateAir.h"
#include "ECS/Player/Animation/States/AnimationStateInAir.h"
#include "ECS/Player/Movement/Movement.h"
#include "MovementStateWall.h"
#include "ECS/Player/PlayerInput.h"

Type MovementStateJump::Check()
{
    // Reset jumps
    if (GetMovement().IsOnGround() || GetCurrentState() == Type::Get<MovementStateWall>())
    {
        if (airJumps)
        {
            LOG("Air jump reset"); 
            airJumps = 0;
        }
        groundJump = false;
    }

    if (GetInput().jumpInput && CanJump())
        return GetType();
    if (GetInput().jumpInputHeld && CanGroundJump())
        return GetType();
    return Type::None();
}

Type MovementStateJump::Update()
{
    if (const auto air = GetState<MovementStateAir>())
        air->Update();
    return Type::Get<MovementStateAir>(); 
}

void MovementStateJump::Enter()
{
    MovementState::Enter();
    
    auto& movement = GetMovement();
    
    // Add wall force
    const bool ground = CanGroundJump();
    const bool wall = CanWallJump();
    const bool air = CanAirJump();

    if (ground || wall)
        groundJump = true;
    else if (air)
    {
        LOG("Air jump!");
        airJumps++;
    }

    auto params = Jump.Get();
    if (!ground && wall)
        if (const auto wallState = GetState<MovementStateWall>())
            params.direction = wallState->GetWallNormal();
    movement.Jump(params);
}

Type MovementStateJump::GetAnimationState() const
{
    return Type::Get<AnimationStateInAir>(); 
}

bool MovementStateJump::CanJump() const
{
    return CanAirJump() || CanWallJump() || CanGroundJump(); 
}

bool MovementStateJump::CanAirJump() const
{
    if (GetMovement().IsOnGround())
        return false;
    if (airJumps < NumAirJumps)
        return true;
    return false;
}

bool MovementStateJump::CanGroundJump() const
{
    if (groundJump)
        return false; 
    const auto& movement = GetMovement(); 
    if (movement.IsOnGround())
        return true;
    if (movement.TimeSinceLeftGround() < CoyoteTime)
        return true;
    return false;
}

bool MovementStateJump::CanWallJump() const
{
    if (groundJump)
        return false; 
    if (GetMovement().IsOnGround())
        return false;
    if (GetCurrentState() == Type::Get<MovementStateWall>())
        return true;
    if (const auto wallState = GetState<MovementStateWall>())
        if (wallState->GetTimeSinceExit() < WallCoyoteTime)
            return true;
    return false;
}
