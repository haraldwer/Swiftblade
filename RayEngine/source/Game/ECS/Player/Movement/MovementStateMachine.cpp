#include "MovementStateMachine.h"

#include "Utility/StateMachine/StateBase.h"


void MovementStateMachine::Init(ECS::EntityID InPlayer)
{
    const auto addState = [&](auto& InState)
    {
        States.emplace_back(reinterpret_cast<StateBase*>(&InState));
        InState.SetPlayer(InPlayer);
    };
    
    addState(Idle); 
    addState(Run); 
    addState(Jump); 
    addState(Air); 
    addState(Crouch); 
    addState(Slide);
    addState(Dash);
    addState(Wall);
    
    StateMachine::Init();
}

void MovementStateMachine::OnBeginContact(const Physics::Contact& InContact)
{
    // How to test this? 
}

void MovementStateMachine::OnEndContact(const Physics::Contact& InContact)
{
}
