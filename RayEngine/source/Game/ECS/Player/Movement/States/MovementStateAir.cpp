#include "MovementStateAir.h"

#include "MovementStateIdle.h"
#include "Game/ECS/Player/Input.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateAir::Check()
{
    if (GetMovement().IsInAir())
        return GetType();
    return Type::None(); 
}

Type MovementStateAir::Update(double InDT)
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    movement.Look(input.RotInput);

    ECS::Movement::MoveParams move;
    move.MovementForce *= 0.7f;
    ECS::Movement::SlowdownParams slowdown;
    slowdown.Slowdown = 0.1f;
    ECS::Movement::VelocityClampParams vel;
    vel.MaxSpeed = 20.0f;
    vel.ClampSlowdown = 0.005f; 
    
    if (!movement.Move(input.MoveInput, move))
        movement.Slowdown(InDT, slowdown);
    movement.VelocityClamp(InDT, vel);
    
    if (!movement.IsInAir())
        return Type::Get<MovementStateIdle>(); 
    return Type::None(); 
}
