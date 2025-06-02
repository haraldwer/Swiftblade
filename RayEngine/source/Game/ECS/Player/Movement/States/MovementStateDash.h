#pragma once
#include "MovementState.h"

class MovementStateDash : public MovementState<MovementStateDash>
{
public:
    Type Check() override;
    Type Update() override;
    void Enter() override;
    int32 Priority() const override { return 7; }

private:
    PROPERTY_C(float, Speed, 55.0f)
    PROPERTY_C(float, Duration, 0.07f)
    PROPERTY_C(float, Cooldown, 0.7f)
    PROPERTY_C(float, UpDirTilt, 0.1f)
    PROPERTY_C(float, InputDeadzone, 0.1f)
    
    Vec3F direction;
};
