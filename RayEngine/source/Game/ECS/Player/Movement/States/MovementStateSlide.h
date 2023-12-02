#pragma once
#include "MovementState.h"
#include "Engine/Property/Property.h"

class MovementStateSlide : public MovementState<MovementStateSlide>
{
    Type Check() override;
    Type Update(double InDT) override;
    void Enter() override;
    void Exit() override;
    int32 Priority() const override { return 4; }

    bool CheckInput(); 
    
    PROPERTY_D(float, Speed, 30.0f);
    PROPERTY_D(float, SpeedThreshold, 15.0f);
    PROPERTY_D(float, InputDeadzone, 0.1f);
    PROPERTY_D(float, InputDot, 0.3);
    PROPERTY_D(float, LookDot, 0.3);

    Vec3F Direction;
};
