#pragma once

#include "PlayerInterface.h"
#include "Engine/ECS/UniqueComponent.h"

namespace ECS
{
    class Input : public UniqueComponent<Input>, public PlayerInterface 
    {
    public:
        
        void Deinit() override;
        void Update(double InDelta) override;
        inline static bool Blocked = false; 

        // TODO: Action mapping and input buffering
        // TODO: Fix weird sensitivity bug
        // TODO: Improve blocking logic
        
        bool JumpInput = false;
        bool CrouchInput = false;
        bool DashInput = false; 
        Vec2F MoveInput;
        Vec2F RotInput;

        int GetPriority() const override { return 10; }
        
    private:
        
        PROPERTY_C(float, Sensitivity, 0.005f);
        
    };
}
