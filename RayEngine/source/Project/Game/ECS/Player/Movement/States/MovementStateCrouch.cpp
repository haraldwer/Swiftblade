#include "MovementStateCrouch.h"

#include "MovementStateIdle.h"
#include "Game/ECS/Player/PlayerInput.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateCrouch::Update(double InDT)
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    ECS::Movement::MoveParams move;
    move.MovementForce *= 0.7f; 
    ECS::Movement::VelocityClampParams vel;
    vel.MaxSpeed *= 0.5f;
    vel.ClampSlowdown *= 0.1f; 
    
    movement.Look(input.RotInput);
    if (!movement.Move(input.MoveInput, move))
        movement.Slowdown(InDT); 
    movement.VelocityClamp(InDT, vel);
    
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
