#include "MovementStateIdle.h"

#include "Game/ECS/Player/Input.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateIdle::Update(double InDT)
{
    const auto& input = GetInput();
    auto& movement = GetMovement();

    movement.Look(input.RotInput);
    movement.Slowdown(InDT);
    movement.VelocityClamp(InDT);
    
    return Type::None(); 
}
