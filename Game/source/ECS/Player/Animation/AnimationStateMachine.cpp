#include "AnimationStateMachine.h"

void AnimationStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        states.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(defaultState); 
    addState(slide); 
    addState(inAir); 
    addState(strike); 
    
    PlayerStateMachine::Init();
}
