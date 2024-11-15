#pragma once
#include "MovementState.h"

class MovementStateAir : public MovementState<MovementStateAir>
{
public: 
    Type Update() override;
    int32 Priority() const override { return 2; }

    Type GetAnimationState() const override;
};
