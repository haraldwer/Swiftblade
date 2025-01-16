#pragma once
#include "ECS/Player/Movement/MovementParams.h"
#include "MovementState.h"

class MovementStateIdle : public MovementState<MovementStateIdle>
{
public:
    Type Update() override;
    int32 Priority() const override { return 0; }
    Type GetAnimationState() const override;
    
private:
    PROPERTY_C(ECS::LookParams, Look, {});
    PROPERTY_C(ECS::SlowdownParams, Slowdown, {});
    PROPERTY_C(ECS::VelocityClampParams, VelocityClamp, {});
};
