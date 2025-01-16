#pragma once

#include "ECS/Player/Movement/MovementParams.h"
#include "MovementState.h"

class MovementStateAir : public MovementState<MovementStateAir>
{
public: 
    Type Update() override;
    int32 Priority() const override { return 2; }
    Type GetAnimationState() const override;

private:
    PROPERTY_C(ECS::LookParams, Look, {});
    PROPERTY_C(ECS::MoveParams, Move, {});
    PROPERTY_C(ECS::SlowdownParams, Slowdown, {});
    PROPERTY_C(ECS::VelocityClampParams, VelocityClamp, {});
};

