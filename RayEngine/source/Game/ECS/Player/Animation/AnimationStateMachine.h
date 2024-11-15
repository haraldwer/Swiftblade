#pragma once
#include "States/AnimationStateDefault.h"
#include "States/AnimationStateInAir.h"
#include "States/AnimationStateSlide.h"
#include "Utility/StateMachine/StateMachine.h"

class AnimationStateMachine : public StateMachine
{
public:
    void Init() override;
    Utility::Type GetDefaultStateType() override { return Utility::GetType<AnimationStateDefault>(); }

private:
    AnimationStateDefault Default; 
    AnimationStateSlide Slide; 
    AnimationStateInAir InAir; 
    
};
