#include "CombatStateMachine.h"

void CombatStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        States.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(Idle); 
    addState(Strike); 
    
    StateMachine::Init();
}
