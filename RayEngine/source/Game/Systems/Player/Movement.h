#pragma once

#include "PlayerInterface.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/ECS/UniqueComponent.h"

namespace ECS
{
    class Input;

    class Movement : public UniqueComponent<Movement>, public PlayerInterface   
    {
    public:
        void Update(double InDelta) override;
        void OnBeginContact(const Physics::Contact& InContact) override;
        
    private:
        void ConsumeRotInput() const;
        void ConsomeMoveInput();
        void ConsumeJumpInput();
        void GroundSnap();

        PROPERTY_C(float, MovementForce, 50.0); 
        PROPERTY_C(float, JumpVelocity, 20.0);
        
        PROPERTY_C(float, GroundDist, 0.2f);
        PROPERTY_C(float, GroundDot, 0.2f);

        // Movement state
        bool OnGround = false;
        Vec3F GroundLocation;  
    };
}
