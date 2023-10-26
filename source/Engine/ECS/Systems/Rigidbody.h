#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct Rigidbody : Component
    {
        // Some exposed properties
        // And pointer to physx object

        
    };

    class SysRigidbody : public System<Rigidbody>
    {
    public:
        
    };
}
