#pragma once
#include "CombatState.h"

class CombatStateStrike : public CombatState<CombatStateStrike>
{
public:
    ~CombatStateStrike() override = default;
    Utility::Type Update() override;
    Utility::Type Check() override;
    Type GetAnimationState() const override;

private:
    PROPERTY_C(float, SweepSize, 1.0f);
    PROPERTY_C(float, StrikeDuration, 1.0f);
};
