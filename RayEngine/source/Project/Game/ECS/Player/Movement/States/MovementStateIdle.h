#pragma once
#include "MovementState.h"

class MovementStateIdle : public MovementState<MovementStateIdle>
{
    Type Update(double InDT) override;
    int32 Priority() const override { return 0; }
};
