#include "MovementStateCrouch.h"

#include "MovementStateIdle.h"
#include "ECS/Player/PlayerInput.h"
#include "ECS/Player/Movement/Movement.h"

Type MovementStateCrouch::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    ECS::Movement::MoveParams move;
    move.MovementForce *= 0.7f; 
    ECS::Movement::VelocityClampParams vel;
    vel.MaxSpeed *= 0.5f;
    vel.ClampSlowdown *= 0.1f; 

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    movement.Look(input.RotInput);
    if (!movement.Move(input.MoveInput, move))
        movement.Slowdown(dt); 
    movement.VelocityClamp(dt, vel);
    
    if (!movement.IsOnGround())
        return Type::Get<MovementStateIdle>();
    if (!movement.IsCrouching())
        return Type::Get<MovementStateIdle>();
    if (!input.CrouchInput)
        return Type::Get<MovementStateIdle>();
    return Type::None(); 
}

void MovementStateCrouch::Enter()
{
    MovementState::Enter(); 
    GetMovement().SetCrouch(true);
}

void MovementStateCrouch::Exit()
{
    MovementState::Exit(); 
    GetMovement().SetCrouch(false); 
}
