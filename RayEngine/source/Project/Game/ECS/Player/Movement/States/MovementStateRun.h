#pragma once
#include "MovementState.h"

class MovementStateRun : public MovementState<MovementStateRun>
{
    Type Update() override;
    int32 Priority() const override { return 1; }
};
