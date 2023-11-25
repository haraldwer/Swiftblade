#pragma once
#include "MovementState.h"

class MovementStateRun : public MovementState<MovementStateRun>
{
    Type Check() override;
    Type Update(double InDT) override;
    int32 Priority() const override { return 1; }
    void Enter() override { LOG("Enter run"); }
};
