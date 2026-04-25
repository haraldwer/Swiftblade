#pragma once

#include "AnimationState.h"

class AnimationStateDefault : public AnimationState<AnimationStateDefault>
{
public: 
    Type Update() override;
};