#pragma once
#include "MovementState.h"

class MovementStateAir : public MovementState<MovementStateAir>
{
public: 
    Type Check() override;
    Type Update(double InDT) override;
    int32 Priority() const override { return 2; }
};
