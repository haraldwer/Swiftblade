#include "MovementStateDash.h"

#include "MovementStateIdle.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Game/ECS/Player/Input.h"

Type MovementStateDash::Check()
{
    auto& input = GetInput(); 
    if (TimeSinceDash() > Cooldown)
        if (input.MoveInput.Length() > InputDeadzone)
            if (input.DashInput)
                return GetType();
    return Type::None(); 
}

Type MovementStateDash::Update(double InDT)
{
    GetRB().SetVelocity(Direction * Speed.Get());
    if (TimeSinceDash() > Duration)
        return Type::Get<MovementStateIdle>();
    return Type::None(); 
}

void MovementStateDash::Enter()
{
    LOG("Enter dash");
    DashTimestamp = GetTime();
    const Vec2F input = GetInput().MoveInput;
    Direction = Vec3F(input.x, UpDirTilt, input.y).GetNormalized();
}

float MovementStateDash::TimeSinceDash() const
{
    return static_cast<float>(GetTime() - DashTimestamp); 
}
