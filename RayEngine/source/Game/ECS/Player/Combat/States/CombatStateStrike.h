#pragma once
#include "CombatState.h"

class CombatStateStrike : public CombatState<CombatStateStrike>
{
public:
    ~CombatStateStrike() override = default;
    Utility::Type Update() override;
    Utility::Type Check() override;
    void Enter() override;

private:
    PROPERTY_C(float, SweepSize, 1.0f);
};
