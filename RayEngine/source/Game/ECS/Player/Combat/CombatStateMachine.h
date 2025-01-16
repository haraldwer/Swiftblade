#pragma once

#include "States/CombatStateIdle.h"
#include "States/CombatStateStrike.h"
#include "Utility/StateMachine/StateMachine.h"
#include "Utility/Type.h"

class CombatStateMachine  : public StateMachine
{
public:
    void Init() override;
    String Name() const override { return "Combat"; }
    Utility::Type GetDefaultStateType() override { return Utility::GetType<CombatStateIdle>(); }

private:
    CombatStateIdle Idle;
    CombatStateStrike Strike;
    
};
