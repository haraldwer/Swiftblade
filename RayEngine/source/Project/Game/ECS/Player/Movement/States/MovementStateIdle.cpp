#include "MovementStateIdle.h"

#include "MovementStateAir.h"
#include "MovementStateRun.h"
#include "Game/ECS/Player/PlayerInput.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateIdle::Update(double InDT)
{
    const auto& input = GetInput();
    auto& movement = GetMovement();

    movement.Look(input.RotInput);
    movement.Slowdown(InDT);
    movement.VelocityClamp(InDT);

    if (movement.IsInAir())
        return Type::Get<MovementStateAir>();

    if (movement.IsOnGround() && input.MoveInput.Length() > 0.5f)
        return Type::Get<MovementStateRun>(); 
    
    return Type::None(); 
}
