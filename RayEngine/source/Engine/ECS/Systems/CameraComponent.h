#pragma once

#include "ECS/System.h"
#include "ECS/Component.h"

namespace ECS
{
    struct CameraComponent : Component<CameraComponent>
    {
        PROPERTY_D(float, FOV, 110.0f);
        PROPERTY_D(float, Near, 0.001f);
        PROPERTY_D(float, Far, 50.0f);
    };

    class SysCameraComponent : public System<CameraComponent>
    {
    public:
        void Update(EntityID InEntity, CameraComponent& InComponent) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -99; }
    };
}
