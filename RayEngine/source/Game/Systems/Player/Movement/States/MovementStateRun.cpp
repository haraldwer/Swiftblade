#include "MovementStateRun.h"

#include "MovementStateIdle.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Game/Systems/Player/Movement/Movement.h"

Type MovementStateRun::Check()
{
    if (!GetMovement().IsOnGround())
        return Type::None(); 
    if (GetRB().GetVelocity().Length() > 0.5f)
        return Type::Get<MovementStateRun>(); 
    return Type::None();
}

Type MovementStateRun::Update(double InDT)
{
    if (GetRB().GetVelocity().Length() < 0.5f)
        return Type::Get<MovementStateIdle>();  
    return Type::None(); 
}

void MovementStateRun::Enter()
{
    LOG("Enter run"); 
}

void MovementStateRun::Exit()
{
    LOG("Exit run"); 
}
