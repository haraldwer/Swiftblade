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
    Utility::Type GetDefaultStateType() override { return Type::Get<AnimationStateDefault>(); }

private:
    AnimationStateDefault default; 
    AnimationStateSlide slide; 
    AnimationStateInAir inAir; 
    AnimationStateStrike strike; 
    
};
