#include "MovementStateIdle.h"

#include "Game/ECS/Player/Animation/States/AnimationStateDefault.h"
#include "Game/ECS/Player/Movement/Movement.h"
#include "MovementStateAir.h"
#include "MovementStateRun.h"

Type MovementStateIdle::Update(double InDT)
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    movement.Look(input.RotInput);
    movement.Slowdown(InDT);
    movement.VelocityClamp(InDT);

    if (movement.IsInAir())
        return Type::Get<MovementStateAir>();

    if (movement.IsOnGround() && input.MoveInput.Length() > 0.5f)
        return Type::Get<MovementStateRun>(); 
    
    return Type::None(); 
}

Type MovementStateIdle::GetAnimationState() const
{
    return Type::Get<AnimationStateDefault>();
}
