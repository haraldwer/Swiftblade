#pragma once
#include "States/AnimationStateIdle.h"
#include "Utility/StateMachine/StateMachine.h"

class AnimationStateMachine : public StateMachine
{
public:

    void Init() override;
    
    Utility::Type GetDefaultStateType() override { return Utility::GetType<AnimationStateIdle>(); }

private:

    AnimationStateIdle Idle; 
    
};
