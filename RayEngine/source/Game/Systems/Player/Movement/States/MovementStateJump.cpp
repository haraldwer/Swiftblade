#include "C:/Dev/Swiftblade/RayEngine/build/CMakeFiles/RayEngine.dir/Debug/cmake_pch.hxx"
#include "MovementStateJump.h"

#include "MovementStateAir.h"
#include "Game/Systems/Player/Input.h"
#include "Game/Systems/Player/Movement/Movement.h"

Type MovementStateJump::Check()
{
    if (GetMovement().IsOnGround() && GetInput().JumpInput)
        return GetType();
    return Type::None();
}

Type MovementStateJump::Update(double InDT)
{
    if (const auto air = GetState<MovementStateAir>())
        air->Update(InDT);
    return Type::Get<MovementStateAir>(); 
}

void MovementStateJump::Enter()
{
    LOG("Enter jump")
    auto& movement = GetMovement();
    movement.Jump();

    // TODO: Maybe do wall jump
}
