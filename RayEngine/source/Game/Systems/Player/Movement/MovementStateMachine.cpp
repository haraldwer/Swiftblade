#include "MovementStateMachine.h"

#include "Utility/StateMachine/StateBase.h"


void MovementStateMachine::Init(ECS::EntityID InPlayer)
{
    States.emplace_back(reinterpret_cast<StateBase*>(&Idle));
    States.emplace_back(reinterpret_cast<StateBase*>(&Run));

    Idle.SetPlayer(InPlayer); 
    Run.SetPlayer(InPlayer); 
    
    StateMachine::Init();
}

void MovementStateMachine::OnBeginContact(const Physics::Contact& InContact)
{
    // How to test this? 
}

void MovementStateMachine::OnEndContact(const Physics::Contact& InContact)
{
}
