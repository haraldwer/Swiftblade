#pragma once
#include "MovementState.h"

class MovementStateRun : public MovementState<MovementStateRun>
{
    Utility::Type Check() override;
    Utility::Type Update(double InDT) override;
    void Enter() override;
    void Exit() override;
};
