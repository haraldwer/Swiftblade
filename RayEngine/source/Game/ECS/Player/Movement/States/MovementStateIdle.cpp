#include "MovementStateIdle.h"

#include "ECS/Player/Animation/States/AnimationStateDefault.h"
#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/PlayerInput.h"
#include "MovementStateAir.h"
#include "MovementStateRun.h"

Type MovementStateIdle::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    movement.Look(input.RotInput);
    movement.Slowdown(dt);
    movement.VelocityClamp(dt);

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
