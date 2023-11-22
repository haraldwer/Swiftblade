#include "C:/Dev/Swiftblade/RayEngine/build/CMakeFiles/RayEngine.dir/Debug/cmake_pch.hxx"
#include "MovementStateAir.h"

#include "MovementStateIdle.h"
#include "Game/Systems/Player/Movement/Movement.h"

Utility::Type MovementStateAir::Check()
{
    if (GetMovement().IsInAir())
        return Type::Get<MovementStateAir>();
    return Type::None(); 
}

Utility::Type MovementStateAir::Update(double InDT)
{
    if (!GetMovement().IsInAir())
        return Type::Get<MovementStateIdle>(); 
    return Type::None(); 
}

void MovementStateAir::Enter()
{
    LOG("Enter air"); 
}

void MovementStateAir::Exit()
{
    LOG("Exit air"); 
}
