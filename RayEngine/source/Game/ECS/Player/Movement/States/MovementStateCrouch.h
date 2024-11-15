#pragma once
#include "MovementState.h"

class MovementStateCrouch : public MovementState<MovementStateCrouch>
{
    Type Update() override;
    void Enter() override;
    void Exit() override;
    int32 Priority() const override { return 3; }
};
