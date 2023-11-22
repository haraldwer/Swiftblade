#pragma once
#include "MovementState.h"

class MovementStateAir : public MovementState<MovementStateAir>
{
    Utility::Type Check() override;
    Utility::Type Update(double InDT) override;
    void Enter() override;
    void Exit() override;
};
