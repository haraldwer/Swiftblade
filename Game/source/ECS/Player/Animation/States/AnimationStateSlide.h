#pragma once
#include "AnimationState.h"

class AnimationStateSlide : public AnimationState<AnimationStateSlide>
{
public:
    Type Update() override;
    
};
