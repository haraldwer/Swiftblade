#pragma once
#include "ECS/Player/PlayerStateMachine.h"
#include "States/AnimationStateDefault.h"
#include "States/AnimationStateInAir.h"
#include "States/AnimationStateSlide.h"
#include "States/AnimationStateStrike.h"

class AnimationStateMachine : public PlayerStateMachine
{
public:
    void Init() override;
    String Name() const override { return "Animation"; }
    Utility::Type GetDefaultStateType() override { return Utility::GetType<AnimationStateDefault>(); }

private:
    AnimationStateDefault Default; 
    AnimationStateSlide Slide; 
    AnimationStateInAir InAir; 
    AnimationStateStrike Strike; 
    
};
