#pragma once

#include "../System.h"

namespace ECS
{
    struct Transform
    {
        Vec3F Position; 
        // Parent?
        // Children? 
    };

    class SysTransform : public System<Transform>
    {
    };
}

