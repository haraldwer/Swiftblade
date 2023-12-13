#include "C:/Dev/Swiftblade/RayEngine/build/CMakeFiles/RayEngine.dir/Debug/cmake_pch.hxx"
#include "MovementStateJump.h"

#include "MovementStateAir.h"
#include "MovementStateWall.h"
#include "Game/ECS/Player/Input.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateJump::Check()
{
    if (GetInput().JumpInput && CanJump())
            return GetType();
    return Type::None();
}

Type MovementStateJump::Update(double InDT)
{
    if (const auto air = GetState<MovementStateAir>())
        air->Update(InDT);
    return Type::Get<MovementStateAir>(); 
}

void MovementStateJump::Enter()
{
    MovementState::Enter();
    
    auto& movement = GetMovement();
    ECS::Movement::JumpParams params;

    // TODO: Maybe do wall jump
    if (GetCurrentState() == Type::Get<MovementStateWall>())
        if (const auto wallState = GetState<MovementStateWall>())
            params.Direction = wallState->GetWallNormal();

    movement.Jump(params);
}

bool MovementStateJump::CanJump()
{
    if (GetMovement().IsOnGround())
        return true;

    // Was recently on ground?

    // Was recently on wall?
    
    return false; 
}
