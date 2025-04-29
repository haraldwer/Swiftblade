#pragma once

#include "ECS/Component.h"
#include "ECS/System.h"

namespace ECS
{
    struct CameraComponent : Component<CameraComponent>
    {
        PROPERTY_D(float, FOV, 110.0f);
        PROPERTY_D(float, Near, 0.001f);
        PROPERTY_D(float, Far, 200.0f);
    };

    class SysCameraComponent : public System<CameraComponent>
    {
    public:
        void Frame(EntityID InEntity, CameraComponent& InComponent) override;
        void Update(EntityID InEntity, CameraComponent& InComponent) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -99; }
    private:
        void Set(EntityID InEntity, CameraComponent& InComponent);
    };
}

