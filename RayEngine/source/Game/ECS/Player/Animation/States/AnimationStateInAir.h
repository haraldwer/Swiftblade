#pragma once

#include "AnimationState.h"

class AnimationStateInAir : public AnimationState<AnimationStateInAir>
{
public:
    Type Update() override;
};
