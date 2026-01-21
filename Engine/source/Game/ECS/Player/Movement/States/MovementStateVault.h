#pragma once
#include "Engine/Physics/Query.h"
#include "MovementState.h"

class MovementStateVault : public MovementState<MovementStateVault>
{
    Type Check() override;
    Type Update() override;
    void Enter() override { LOG("Enter vault"); }
    void Exit() override;
    int32 Priority() const override { return 3; };

    Physics::QueryResult Sweep() const; 
    
    PROPERTY_C(float, ForwardDist, 1.5f)
    PROPERTY_C(float, SweepHeight, 1.0f);
    PROPERTY_C(float, SweepRadius, 0.5f);
    PROPERTY_C(float, Deadzone, 0.1f);
    PROPERTY_C(float, InputDot, 0.3f);
    PROPERTY_C(float, UpSpeed, 25.0f);
    PROPERTY_C(float, HeightOffset, 2.0f);
    
    Vec3F ledgeLocation;
    Vec3F sweepDirection; 
    
};
