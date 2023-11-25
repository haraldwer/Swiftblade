#pragma once
#include "MovementState.h"

class MovementStateJump : public MovementState<MovementStateJump>
{
    Type Check() override;
    Type Update(double InDT) override;
    void Enter() override;
    int32 Priority() const override { return 6; }
};

