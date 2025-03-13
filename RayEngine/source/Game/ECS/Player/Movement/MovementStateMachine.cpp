#include "MovementStateMachine.h"

#include "Core/StateMachine/StateBase.h"

void MovementStateMachine::Init()
{
    const auto addState = [&](auto& InState)
    {
        States.emplace_back(reinterpret_cast<StateBase*>(&InState));
    };
    
    addState(Idle); 
    addState(Run); 
    addState(Jump); 
    addState(Air); 
    addState(Crouch); 
    addState(Slide);
    addState(Dash);
    addState(Wall);
    addState(Vault);
    
    PlayerStateMachine::Init();
}
