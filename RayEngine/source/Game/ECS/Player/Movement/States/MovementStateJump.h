#pragma once

#include "MovementState.h"
#include "ECS/Player/Movement/MovementParams.h"

class MovementStateJump : public MovementState<MovementStateJump>
{
public:
    Type Check() override;
    Type Update() override;
    void Enter() override;
    int32 Priority() const override { return 6; }
    Type GetAnimationState() const override;
    
    bool CanJump() const;
    bool CanAirJump() const;
    bool CanGroundJump() const; 
    bool CanWallJump() const;

private:
    PROPERTY_C(float, CoyoteTime, 0.2f);
    PROPERTY_C(float, WallCoyoteTime, 0.4f);
    PROPERTY_C(int, NumAirJumps, 1);
    PROPERTY_C(ECS::JumpParams, Jump, {});
    
    int airJumps = 0;
    bool groundJump = false;
    
};

