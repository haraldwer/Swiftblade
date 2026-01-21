#pragma once

#include "AnimationState.h"

class AnimationStateStrike : public AnimationState<AnimationStateStrike>
{
public:
    Utility::Type Update() override;
    int32 Priority() const override { return 1; }

private:
    PROPERTY_C(float, Duration, 1.0f);
    PROPERTY_C(float, LerpPow, 2.0f);
};
