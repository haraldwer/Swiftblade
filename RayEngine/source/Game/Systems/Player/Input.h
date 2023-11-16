#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Input : Component<Input>
    {
        PROPERTY_C(float, Sensitivity, 0.005f);
    };

    class SysInput : public System<Input>
    {
    public:
        void Deinit(EntityID InID, Input& InComponent) override;
        void Update(EntityID InID, Input& InComponent, double InDelta) override;
    };
}
