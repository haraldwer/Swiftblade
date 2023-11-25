#pragma once
#include "MovementState.h"
#include "Engine/Property/Property.h"

class MovementStateWall : public MovementState<MovementStateWall>
{
    Type Check() override;
    Type Update(double InDT) override;
    void Enter() override;
    void Exit() override;
    int32 Priority() const override { return 5; }
    
    bool CheckWall();
    bool CheckInput();

    PROPERTY_D(float, InputThreshold, 0.1f);
    PROPERTY_D(float, SweepLength, 0.3f);
    PROPERTY_D(float, MaxVerticalDot, 0.3f);
    PROPERTY_D(float, NormalInterpSpeed, 10.0f);

    Vec3F TargetWallNormal;
    Vec3F CurrentWallNormal;
    bool OnWall = false; 
    
};
