#include "MovementStateDash.h"

#include "MovementStateIdle.h"
#include "ECS/Player/PlayerInput.h"
#include "Engine/ECS/Systems/Rigidbody.h"

Type MovementStateDash::Check()
{
    const auto& input = GetInput(); 
    if (input.dashInput)
        if (GetTimeSinceExit() > Cooldown)
            if (input.moveInput.Length() > InputDeadzone)
                return GetType();
    return Type::None(); 
}

Type MovementStateDash::Update()
{
    GetRB().SetVelocity(direction * Speed.Get());
    if (GetTimeSinceEnter() > Duration)
        return Type::Get<MovementStateIdle>();
    return Type::None(); 
}

void MovementStateDash::Enter()
{
    MovementState::Enter(); 
    const Vec2F input = GetInput().moveInput;
    direction = Vec3F(input.x, UpDirTilt, input.y).GetNormalized();
}
