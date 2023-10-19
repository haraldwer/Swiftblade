#pragma once

#include "../System.h"

namespace ECS
{
    struct Camera
    {
        float FOV = 90.0f;
    };

    class SysCamera : public System<Camera>
    {
    public:
        void Init(EntityID InEntity, Camera& InComponent) override;
        void Update(EntityID InEntity, Camera& InComponent, double InDelta) override;
    };
    
}

