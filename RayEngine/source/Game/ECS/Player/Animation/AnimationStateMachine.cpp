#include "AnimationStateMachine.h"

void AnimationStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        States.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(Default); 
    addState(Slide); 
    addState(InAir); 
    addState(Strike); 
    
    PlayerStateMachine::Init();
}
