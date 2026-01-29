#pragma once

#include "ECS/Component.h"
#include "ECS/System.h"

namespace ECS
{
    struct Camera : Component<Camera>
    {
        PROPERTY_D(float, FOV, 110.0f);
        PROPERTY_D(float, Near, 0.001f);
        PROPERTY_D(float, Far, 50.0f);
    };

    class SysCamera : public System<Camera>
    {
    public:
        void Frame(EntityID InEntity, Camera& InComponent) override;
        void Tick(EntityID InEntity, Camera& InComponent) override;
        bool ShouldTick() const override { return true; }
        int GetPriority() const override { return -99; }
    private:
        void Set(EntityID InEntity, Camera& InComponent);
    };
}

