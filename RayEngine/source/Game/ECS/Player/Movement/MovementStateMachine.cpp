#include "MovementStateMachine.h"

#include "Core/StateMachine/StateBase.h"

void MovementStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        states.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(idle); 
    addState(run); 
    addState(jump); 
    addState(air); 
    addState(crouch); 
    addState(slide);
    addState(dash);
    addState(wall);
    addState(vault);
    
    PlayerStateMachine::Init();
}
