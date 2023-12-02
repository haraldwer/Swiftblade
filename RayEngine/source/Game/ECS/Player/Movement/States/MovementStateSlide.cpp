#include "MovementStateSlide.h"

#include "MovementStateCrouch.h"
#include "MovementStateIdle.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/ECS/Player/Input.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateSlide::Check()
{
    const auto& input = GetInput();
    if (GetMovement().IsOnGround() && input.CrouchInput)
    {
        if (GetRB().GetVelocity().length() > SpeedThreshold &&
            input.MoveInput.Length() > InputDeadzone)
            return GetType(); 
        return Type::Get<MovementStateCrouch>();
    }
    return Type::None(); 
}

Type MovementStateSlide::Update(double InDT)
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();
    
    movement.Look(input.RotInput);
    GetRB().SetVelocity(Direction * Speed.Get());
    
    if (!movement.IsOnGround())
    {
        LOG("Exit side becaus not on ground")
        return Type::Get<MovementStateIdle>();
    }
    if (!movement.IsCrouching())
    {
        LOG("Exit side becaus not crouching")
        return Type::Get<MovementStateIdle>();
    }
    if (!input.CrouchInput)
    {
        LOG("Exit side becaus does not want to crouch")
        return Type::Get<MovementStateIdle>();
    }
    if (!CheckInput())
    {
        LOG("Exit side becaus no input")
        return Type::Get<MovementStateIdle>();
    }
    
    return Type::None(); 
}

void MovementStateSlide::Enter()
{
    GetMovement().SetCrouch(true);
    Direction = (GetRB().GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();
    LOG("Enter slide");
}

void MovementStateSlide::Exit()
{
    GetMovement().SetCrouch(false);
    LOG("Exit slide");
}

bool MovementStateSlide::CheckInput()
{
    auto input = GetInput().MoveInput; 
    
    // Check input length
    if (input.Length() < InputDeadzone)
        return false;
    
    // Check input dot
    const Vec3F dir = Vec3F(input.x, 0.0f, input.y).GetNormalized();
    if (dir.Dot(Direction) < InputDot)
        return false; 

    return true; 
}
