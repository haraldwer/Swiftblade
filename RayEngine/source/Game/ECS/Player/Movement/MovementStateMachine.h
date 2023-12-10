#pragma once

#include "Utility/StateMachine/StateMachine.h"
#include "States/MovementStateAir.h"
#include "States/MovementStateCrouch.h"
#include "States/MovementStateDash.h"
#include "States/MovementStateIdle.h"
#include "States/MovementStateJump.h"
#include "States/MovementStateRun.h"
#include "States/MovementStateSlide.h"
#include "States/MovementStateWall.h"

namespace Physics
{
    struct Contact;
}

class MovementStateMachine : public StateMachine
{
public:

    void Init();
    
    Utility::Type GetDefaultStateType() override { return Utility::GetType<MovementStateIdle>(); }    
    
    void OnBeginContact(const Physics::Contact& InContact);
    void OnEndContact(const Physics::Contact& InContact);

private:

    MovementStateIdle Idle;
    MovementStateRun Run;
    MovementStateJump Jump;
    MovementStateAir Air;
    MovementStateCrouch Crouch;
    MovementStateSlide Slide;
    MovementStateDash Dash;
    MovementStateWall Wall;
};
