#include "MovementStateRun.h"

#include "MovementStateIdle.h"
#include "ECS/Player/PlayerInput.h"
#include "ECS/Player/Movement/Movement.h"

Type MovementStateRun::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    movement.Look(input.rotInput, Look);
    if (!movement.Move(input.moveInput, Move))
        movement.Slowdown(dt, Slowdown); 
    movement.VelocityClamp(dt, VelocityClamp);
    
    if (input.moveInput.Length() < 0.5f)
        return Type::Get<MovementStateIdle>();
    
    return Type::None(); 
}
