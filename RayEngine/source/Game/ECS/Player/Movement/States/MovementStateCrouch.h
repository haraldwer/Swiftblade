#pragma once
#include "MovementState.h"

class MovementStateCrouch : public MovementState<MovementStateCrouch>
{
public:
    Type Update() override;
    void Enter() override;
    void Exit() override;
    int32 Priority() const override { return 3; }
    
private:
    PROPERTY_C(ECS::LookParams, Look, {});
    PROPERTY_C(ECS::MoveParams, Move, {});
    PROPERTY_C(ECS::SlowdownParams, Slowdown, {});
    PROPERTY_C(ECS::VelocityClampParams, VelocityClamp, {});
    
};
