#pragma once

#include "ECS/Player/PlayerInterface.h"
#include "Engine/ECS/UniqueComponent.h"
#include "MovementParams.h"

class MovementStateMachine;

namespace ECS
{
    class PlayerInput;

    class Movement : public UniqueComponent<Movement>, public PlayerInterface
    {
    public:
        void Init() override;
        void Deinit() override;
        void Update() override;
        void OnBeginContact(const Physics::Contact& InContact) override;
        bool Edit(const String& InName = "") override;
        
        int GetPriority() const override { return 9; }

        bool IsOnGround() const { return OnGround; }
        bool IsInAir() const { return !IsOnGround(); }
        bool IsCrouching() const { return Crouching; } 

        void Look(const Vec2F& InInput, const LookParams& InParams = LookParams()) const;
        bool Move(const Vec2F& InInput, const MoveParams& InParams = MoveParams()) const;
        void Jump(const JumpParams& InParams = JumpParams());
        void Slowdown(double InDelta, const SlowdownParams& InParams = SlowdownParams()) const;
        void VelocityClamp(double InDelta, const VelocityClampParams& InParams = VelocityClampParams()) const;
        void SetCrouch(bool InCrouch, const CrouchParams& InParams = CrouchParams());

        double TimeSinceJump() const;
        double TimeSinceLeftGround() const;  
        
    private:
        
        void GroundSnap();
        bool CheckGroundHit(const Vec3F& InNormal) const;

        PROPERTY_C(float, GroundDist, 0.5f)
        PROPERTY_C(float, GroundDot, 0.2f)
        PROPERTY_C(float, GroundJumpDelay, 0.5f)

        // Movement state
        bool OnGround = true;
        bool Crouching = false;
        double GroundTimestamp = 0.0f;

        ObjectPtr<MovementStateMachine> StateMachine; 
    };
}
