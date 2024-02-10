#pragma once

#include "AnimationState.h"

class AnimationStateIdle : public AnimationState<AnimationStateIdle>
{
public: 
    Type Update(double InDT) override;
    
private:
    float CurrentRoll = 0.0f; 
};