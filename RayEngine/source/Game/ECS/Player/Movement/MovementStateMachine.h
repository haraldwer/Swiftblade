#pragma once

#include "ECS/Player/PlayerStateMachine.h"
#include "States/MovementStateAir.h"
#include "States/MovementStateCrouch.h"
#include "States/MovementStateDash.h"
#include "States/MovementStateIdle.h"
#include "States/MovementStateJump.h"
#include "States/MovementStateRun.h"
#include "States/MovementStateSlide.h"
#include "States/MovementStateVault.h"
#include "States/MovementStateWall.h"

namespace Physics
{
    struct Contact;
}

class MovementStateMachine : public PlayerStateMachine
{
public:
    void Init() override;
    String Name() const override { return "Movement"; }
    Utility::Type GetDefaultStateType() override { return Type::Get<MovementStateIdle>(); }    
    
private:
    MovementStateIdle idle;
    MovementStateRun run;
    MovementStateJump jump;
    MovementStateAir air;
    MovementStateCrouch crouch;
    MovementStateSlide slide;
    MovementStateDash dash;
    MovementStateWall wall;
    MovementStateVault vault;
};
