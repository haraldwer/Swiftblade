#pragma once
#include "ECS/Component.h"
#include "ECS/System.h"

namespace ECS
{
    struct Light : Component<Light>
    {
        PROPERTY_D(Vec3F, Color, Vec3F::One());
        PROPERTY_D(float, Range, 10.0f);
        PROPERTY_D(float, Radius, 50.0f);
        PROPERTY_D(float, Intensity, 1.0f);
        PROPERTY_D(bool, Shadows, true);
        PROPERTY_D(bool, Visible, true);
        uint32 id = 0;
    };

    class SysLight : public System<Light>
    {
    public:
        void Frame(EntityID InID, Light& InComponent) override;
        bool ShouldTick() const override { return true; }
        int GetPriority() const override { return -100; }
    };
}
