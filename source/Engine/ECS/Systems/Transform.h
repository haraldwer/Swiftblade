#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Transform : Component
    {
        PROPERTY(Vec3F, Position); 
        // Parent?
        // Children? 
    };

    class SysTransform : public System<Transform>
    {
    };
}

