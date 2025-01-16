#pragma once

#include "Engine/ECS/UniqueComponent.h"
#include "PlayerInterface.h"

namespace ECS
{
    class PlayerInput : public UniqueComponent<PlayerInput>, public PlayerInterface 
    {
    public:
        
        void Update() override;

        // TODO: input buffering
        
        bool JumpInput = false;
        bool JumpInputHeld = false;
        bool CrouchInput = false;
        bool DashInput = false; 
        Vec2F MoveInput;
        Vec2F RotInput;

        int GetPriority() const override { return 10; }
        
    private:
        
        PROPERTY_C(float, Sensitivity, 0.0025f);
        
    };
}
