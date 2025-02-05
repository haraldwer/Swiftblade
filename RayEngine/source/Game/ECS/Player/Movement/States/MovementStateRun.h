#pragma once
#include "MovementState.h"
#include "ECS/Player/Movement/MovementParams.h"

class MovementStateRun : public MovementState<MovementStateRun>
{
public:
    Type Update() override;
    int32 Priority() const override { return 1; }

private:
    PROPERTY_C(ECS::LookParams, Look, {});
    PROPERTY_C(ECS::MoveParams, Move, {});
    PROPERTY_C(ECS::SlowdownParams, Slowdown, {});
    PROPERTY_C(ECS::VelocityClampParams, VelocityClamp, {});
};
