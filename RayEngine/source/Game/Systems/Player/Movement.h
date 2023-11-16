#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Movement : Component<Movement>
    {
        PROPERTY_C(float, MovementForce, 50.0); 
        PROPERTY_C(float, JumpForce, 20.0);

        bool JumpInput = false;
        Vec2F MoveInput;
        Vec2F RotInput;
    };

    class SysMovement : public System<Movement>
    {
    public:
        void Update(EntityID InID, Movement& InComponent, double InDelta) override;
    private:
        void ConsumeRotInput(ECS::Movement& InMovement);
        void ConsomeMoveInput(ECS::Movement& InMovement);
        void ConsumeJumpInput(ECS::Movement& InMovement);
    };
}
