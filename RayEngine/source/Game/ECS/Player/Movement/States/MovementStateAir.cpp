#include "MovementStateAir.h"

#include "ECS/Player/Animation/States/AnimationStateInAir.h"
#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/PlayerInput.h"
#include "MovementStateIdle.h"

Type MovementStateAir::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    movement.Look(input.rotInput, Look);
    if (!movement.Move(input.moveInput, Move))
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
