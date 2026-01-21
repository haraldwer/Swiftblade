#include "CombatStateMachine.h"

void CombatStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        states.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(idle); 
    addState(strike); 
    
    PlayerStateMachine::Init();
}
