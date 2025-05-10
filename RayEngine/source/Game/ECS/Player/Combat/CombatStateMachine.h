#pragma once

#include "ECS/Player/PlayerStateMachine.h"
#include "States/CombatStateIdle.h"
#include "States/CombatStateStrike.h"
#include "Utility/Type.h"

class CombatStateMachine  : public PlayerStateMachine
{
public:
    void Init() override;
    String Name() const override { return "Combat"; }
    Utility::Type GetDefaultStateType() override { return Type::Get<CombatStateIdle>(); }

private:
    CombatStateIdle Idle;
    CombatStateStrike Strike;
    
};
