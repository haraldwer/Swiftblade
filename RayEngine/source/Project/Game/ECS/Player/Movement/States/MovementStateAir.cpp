#include "MovementStateAir.h"

#include "Game/ECS/Player/Movement/Movement.h"
#include "Game/ECS/Player/PlayerInput.h"
#include "MovementStateIdle.h"
#include "Game/ECS/Player/Animation/States/AnimationStateInAir.h"

Type MovementStateAir::Update()
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

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    if (!movement.Move(input.MoveInput, move))
        movement.Slowdown(dt, slowdown);
    movement.VelocityClamp(dt, vel);
    
    if (!movement.IsInAir())
        return Type::Get<MovementStateIdle>(); 
    return Type::None(); 
}

Type MovementStateAir::GetAnimationState() const
{
    return Type::Get<AnimationStateInAir>();
}
