#pragma once
#include "CombatState.h"

class CombatStateIdle : public CombatState<CombatStateIdle>
{
public:
    ~CombatStateIdle() override = default;
};
