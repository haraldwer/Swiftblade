#include "MovementStateRun.h"

#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateRun::Update(double InDT)
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    movement.Look(input.RotInput);
    if (!movement.Move(input.MoveInput))
        movement.Slowdown(InDT); 
    movement.VelocityClamp(InDT);
    
    if (input.MoveInput.Length() < 0.5f)
        return Type::Get<MovementStateIdle>();
    
    return Type::None(); 
}
