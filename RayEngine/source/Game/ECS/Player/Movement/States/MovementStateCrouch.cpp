#include "MovementStateCrouch.h"

#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/PlayerInput.h"
#include "MovementStateIdle.h"

Type MovementStateCrouch::Update()
{
    const auto& input = GetInput();
    const auto& movement = GetMovement();

    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    movement.Look(input.rotInput, Look);
    if (!movement.Move(input.moveInput, Move))
        movement.Slowdown(dt, Slowdown); 
    movement.VelocityClamp(dt, VelocityClamp);
    
    if (!movement.IsOnGround())
        return Type::Get<MovementStateIdle>();
    if (!movement.IsCrouching())
        return Type::Get<MovementStateIdle>();
    if (!input.crouchInput)
        return Type::Get<MovementStateIdle>();
    return Type::None(); 
}

void MovementStateCrouch::Enter()
{
    MovementState::Enter(); 
    GetMovement().SetCrouch(true);
}

void MovementStateCrouch::Exit()
{
    MovementState::Exit(); 
    GetMovement().SetCrouch(false); 
}
