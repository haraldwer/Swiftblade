#include "MovementStateAir.h"

#include "ECS/Player/Animation/States/AnimationStateInAir.h"
#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/PlayerInput.h"
#include "MovementStateIdle.h"

Type MovementStateAir::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    movement.Look(input.RotInput, Look);

    //move.MovementForce *= 0.7f;
    //slowdown.Slowdown = 0.1f;
    //vel.MaxSpeed = 20.0f;
    //vel.ClampSlowdown = 0.005f; 

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    if (!movement.Move(input.MoveInput, Move))
        movement.Slowdown(dt, Slowdown);
    movement.VelocityClamp(dt, VelocityClamp);
    
    if (!movement.IsInAir())
        return Type::Get<MovementStateIdle>(); 
    return Type::None(); 
}

Type MovementStateAir::GetAnimationState() const
{
    return Type::Get<AnimationStateInAir>();
}
