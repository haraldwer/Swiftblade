#pragma once

#include "Game/Systems/Player/PlayerInterface.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/ECS/UniqueComponent.h"

class MovementStateMachine;

namespace ECS
{
    class Input;

    class Movement : public UniqueComponent<Movement>, public PlayerInterface   
    {
    public:
        void Init() override;
        void Deinit() override;
        void Update(double InDelta) override;
        void OnBeginContact(const Physics::Contact& InContact) override;

        int GetPriority() const override { return 9; }

        bool IsOnGround() const { return OnGround; }
        bool IsInAir() const { return !IsOnGround(); }
        
    private:
        void ConsumeRotInput() const;
        void ConsomeMoveInput(); 
        void ConsumeJumpInput();
        void ApplySlowdown(double InDelta);
        void ApplyVelocityClamp(double InDelta) const;
        void GroundSnap();

        // Movement forces
        PROPERTY_C(float, MovementForce, 180.0f);
        PROPERTY_C(float, AirMovementMultiplier, 0.7f);
        PROPERTY_C(float, JumpVelocity, 22.0f);

        // Speed clamps
        PROPERTY_C(float, MaxGroundSpeed, 15.0f);
        PROPERTY_C(float, MaxAirSpeed, 20.0f);
        PROPERTY_C(float, AirClampSlowdown, 0.005f);
        PROPERTY_C(float, GroundClampSlowdown, 0.001f);
        PROPERTY_C(float, MaxVerticalSpeed, 30.0f);

        // Friction / Slowdown
        PROPERTY_C(float, GroundSlowdown, 0.001f);
        PROPERTY_C(float, AirSlowdown, 0.1f);

        // Ground detection
        PROPERTY_C(float, GroundDist, 0.5f);
        PROPERTY_C(float, GroundDot, 0.2f);

        // Movement state
        bool OnGround = false;
        Vec3F GroundLocation;
        Vec2F LastInputVector; 

        ObjectPtr<MovementStateMachine> StateMachine; 
    };
}
