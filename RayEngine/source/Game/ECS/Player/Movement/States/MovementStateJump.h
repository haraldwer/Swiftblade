﻿#pragma once
#include "Engine/Property/Property.h"
#include "MovementState.h"

class MovementStateJump : public MovementState<MovementStateJump>
{
    Type Check() override;
    Type Update() override;
    void Enter() override;
    int32 Priority() const override { return 6; }
    Type GetAnimationState() const override;
    
    bool CanJump() const;
    bool CanAirJump() const;
    bool CanGroundJump() const; 
    bool CanWallJump() const;
    
    PROPERTY_C(float, CoyoteTime, 0.2f);
    PROPERTY_C(float, WallCoyoteTime, 0.4f);
    PROPERTY_C(int, NumAirJumps, 1);
    
    int AirJumps = 0;
    bool GroundJump = false;
    
};
