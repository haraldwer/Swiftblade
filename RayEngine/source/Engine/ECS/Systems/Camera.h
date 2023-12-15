#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Camera : Component<Camera>
    {
        PROPERTY_D(float, FOV, 100.0f);
    };

    class SysCamera : public System<Camera>
    {
    public:
        void Update(EntityID InEntity, Camera& InComponent, double InDelta) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -99; }
    };
}

