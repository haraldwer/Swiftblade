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
            Vec3F position;
            Vec3F impulse;
            Vec3F normal;
            float separation;
        };
        
        ECS::EntityID target = ECS::INVALID_ID;
        ECS::EntityID self = ECS::INVALID_ID;
        Vector<Point> points; 
        bool isTrigger = false;
    };
}
