#pragma once

#include "Engine/ECS/UniqueComponent.h"
#include "PlayerInterface.h"

namespace ECS
{
    class PlayerInput : public UniqueComponent<PlayerInput>, public PlayerInterface 
    {
    public:
        
        void Tick() override;

        // TODO: input buffering
        
        bool jumpInput = false;
        bool jumpInputHeld = false;
        bool crouchInput = false;
        bool dashInput = false; 
        Vec2F moveInput;
        Vec2F rotInput;

        int GetPriority() const override { return 10; }
        
    private:
        
        PROPERTY_C(float, Sensitivity, 0.0025f);
        
    };
}
