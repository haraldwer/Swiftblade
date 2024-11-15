#pragma once

#include "AnimationState.h"

class AnimationStateDefault : public AnimationState<AnimationStateDefault>
{
public: 
    Type Update() override;
    
private:
    float CurrentRoll = 0.0f; 
    float CurrentHeight = 0.0f; 
};