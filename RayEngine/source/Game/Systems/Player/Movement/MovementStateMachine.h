#pragma once
#include "States/MovementStateIdle.h"
#include "States/MovementStateRun.h"
#include "Utility/StateMachine/StateMachine.h"

namespace Physics
{
    struct Contact;
}

class MovementStateMachine : public StateMachine
{
public:

    void Init(ECS::EntityID InPlayer);
    
    Utility::Type GetDefaultStateType() override { return Utility::GetType<MovementStateIdle>(); }    
    
    void OnBeginContact(const Physics::Contact& InContact);
    void OnEndContact(const Physics::Contact& InContact);

private:

    MovementStateIdle Idle;
    MovementStateRun Run;
};
