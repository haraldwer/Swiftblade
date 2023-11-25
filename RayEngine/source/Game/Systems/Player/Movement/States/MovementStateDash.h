#pragma once
#include "MovementState.h"
#include "Engine/Property/Property.h"

class MovementStateDash : public MovementState<MovementStateDash>
{
    Type Check() override;
    Type Update(double InDT) override;
    void Enter() override;
    int32 Priority() const override { return 7; }
    
    float TimeSinceDash() const; 
    
    PROPERTY_D(float, Speed, 70.0f)
    PROPERTY_D(float, Duration, 1.0f)
    PROPERTY_D(float, Cooldown, 1.0f)
    PROPERTY_D(float, UpDirTilt, 0.1f)
    PROPERTY_D(float, InputDeadzone, 0.1f)
    
    Vec3F Direction;
    double DashTimestamp = 0.0f; 
};
