#pragma once
#include "MovementState.h"
#include "Engine/Property/Property.h"

class MovementStateWall : public MovementState<MovementStateWall>
{
public:
    
    Vec3F GetWallNormal() const { return CurrentWallNormal; }

private: 
    
    Type Check() override;
    Type Update(double InDT) override;
    void Enter() override;
    void Exit() override;
    int32 Priority() const override { return 5; }
    
    bool CheckWall();
    bool CheckInput() const;

    PROPERTY_C(float, InputThreshold, 0.1f);
    PROPERTY_C(float, VelocityThreshold, 2.0f);
    PROPERTY_C(float, SweepLength, 0.3f);
    PROPERTY_C(float, MaxVerticalDot, 0.3f);
    PROPERTY_C(float, MinWallInputDot, -0.3f);
    PROPERTY_C(float, NormalInterpSpeed, 10.0f);
    PROPERTY_C(float, EnterAirDelay, 0.2f);

    Vec3F TargetWallNormal;
    Vec3F CurrentWallNormal;
    bool OnWall = false; 
    
};
