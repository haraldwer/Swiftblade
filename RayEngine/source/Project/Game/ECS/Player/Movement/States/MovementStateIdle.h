#pragma once
#include "MovementState.h"

class MovementStateIdle : public MovementState<MovementStateIdle>
{
    Type Update() override;
    int32 Priority() const override { return 0; }
    Type GetAnimationState() const override;
};
