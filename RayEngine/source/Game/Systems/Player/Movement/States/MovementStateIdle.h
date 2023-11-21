#pragma once
#include "MovementState.h"

class MovementStateIdle : public MovementState<MovementStateIdle>
{
    void Enter() override;
    void Exit() override;
};
