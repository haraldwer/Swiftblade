#pragma once
#include "MovementState.h"
#include "Engine/Property/Property.h"

class MovementStateDash : public MovementState<MovementStateDash>
{
    Type Check() override;
    Type Update(double InDT) override;
    void Enter() override;
    int32 Priority() const override { return 7; }
    
    PROPERTY_C(float, Speed, 55.0f)
    PROPERTY_C(float, Duration, 0.07f)
    PROPERTY_C(float, Cooldown, 0.7f)
    PROPERTY_C(float, UpDirTilt, 0.1f)
    PROPERTY_C(float, InputDeadzone, 0.1f)
    
    Vec3F Direction;
};
