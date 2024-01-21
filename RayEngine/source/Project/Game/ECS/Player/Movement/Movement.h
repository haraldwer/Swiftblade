#pragma once

#include "Game/ECS/Player/PlayerInterface.h"
#include "Engine/ECS/UniqueComponent.h"
#include "Utility/Time/Time.h"

class MovementStateMachine;

namespace ECS
{
    class PlayerInput;

    class Movement : public UniqueComponent<Movement>, public PlayerInterface   
    {
    public:
        void Init() override;
        void Deinit() override;
        void Update(double InDelta) override;
        void OnBeginContact(const Physics::Contact& InContact) override;

        bool Edit(const String& InName = "") override;
        void DebugDraw() const;
        
        int GetPriority() const override { return 9; }

        bool IsOnGround() const { return OnGround; }
        bool IsInAir() const { return !IsOnGround(); }
        bool IsCrouching() const { return Crouching; } 

        struct LookParams
        {
            Vec2F SensitivityMultiplier = 1.0f; 
        };
        void Look(const Vec2F& InInput, const LookParams& InParams = LookParams()) const;

        struct MoveParams
        {
            float InputDeadzone = 0.1f; 
            float MovementForce = 150.0f;
            Vec3F PlaneNormal = Vec3F::Zero();
        };
        bool Move(const Vec2F& InInput, const MoveParams& InParams = MoveParams()) const;

        struct JumpParams
        {
            float UpVelocity = 22.0f;
            float DirectionalForce = 20.0f;
            Vec3F Direction = Vec3F::Zero();
        };
        void Jump(const JumpParams& InParams = JumpParams());

        struct SlowdownParams
        {
            float Slowdown = 0.0005f; 
        };
        void Slowdown(double InDelta, const SlowdownParams& InParams = SlowdownParams()) const;

        struct VelocityClampParams
        {
            float MaxSpeed = 15.0f;
            float MaxVerticalSpeed = 50.0f;
            float ClampSlowdown = 0.001f;
        };
        void VelocityClamp(double InDelta, const VelocityClampParams& InParams = VelocityClampParams()) const;
        
        struct CrouchParams
        {
            float HeightMul = 0.3f; 
        };
        void SetCrouch(bool InCrouch, const CrouchParams& InParams = CrouchParams());

        double TimeSinceJump() const;
        double TimeSinceLeftGround() const;  
        
    private:
        
        void GroundSnap();
        bool CheckGroundHit(const Vec3F& InNormal) const;
        
        inline static constexpr float GroundDist = 0.5f;
        inline static constexpr float GroundDot = 0.2f;
        inline static constexpr float GroundJumpDelay = 0.5f;

        // Movement state
        bool OnGround = true;
        bool Crouching = false;
        double GroundTimestamp = 0.0f;

        ObjectPtr<MovementStateMachine> StateMachine; 
    };
}
