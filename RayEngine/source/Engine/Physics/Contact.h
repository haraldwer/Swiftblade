#pragma once
#include "ECS/Entity.h"

namespace Physics
{
    struct Contact
    {
        enum class Event : uint8
        {
            BEGIN,
            END
        };
        
        struct Point
        {
            Vec3F Position;
            Vec3F Impulse;
            Vec3F Normal;
            float Separation;
        };
        
        ECS::EntityID Target = ECS::InvalidID;
        ECS::EntityID Self = ECS::InvalidID;
        Vector<Point> Points; 
        bool IsTrigger = false;
    };
}
