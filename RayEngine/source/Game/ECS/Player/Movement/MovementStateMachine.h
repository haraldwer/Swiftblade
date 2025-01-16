#pragma once

#include "States/MovementStateAir.h"
#include "States/MovementStateCrouch.h"
#include "States/MovementStateDash.h"
#include "States/MovementStateIdle.h"
#include "States/MovementStateJump.h"
#include "States/MovementStateRun.h"
#include "States/MovementStateSlide.h"
#include "States/MovementStateVault.h"
#include "States/MovementStateWall.h"
#include "Utility/StateMachine/StateMachine.h"

namespace Physics
{
    struct Contact;
}

class MovementStateMachine : public StateMachine
{
public:
    void Init() override;
    Utility::Type GetDefaultStateType() override { return Utility::GetType<MovementStateIdle>(); }    
    
private:
    MovementStateIdle Idle;
    MovementStateRun Run;
    MovementStateJump Jump;
    MovementStateAir Air;
    MovementStateCrouch Crouch;
    MovementStateSlide Slide;
    MovementStateDash Dash;
    MovementStateWall Wall;
    MovementStateVault Vault;
};
