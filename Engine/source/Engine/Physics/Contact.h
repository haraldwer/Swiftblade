#pragma once
#include "ECS/Entity.h"

namespace Physics
{
    struct Contact
    {
        enum class Event : uint8
        {
            BEGIN = 0,
            STAY,
            END
        };
        
        struct Point
        {
            Vec3F position;
            Vec3F firstPoint;
            Vec3F secondPoint;
            Vec3F impulse;
            Vec3F normal;
            float separation;
        };
        
        ECS::EntityID first = ECS::INVALID_ID;
        ECS::EntityID second = ECS::INVALID_ID;
        Vector<Point> points;
        Event type;
        bool isTrigger = false;

        ECS::EntityID GetOther(const ECS::EntityID InID)
        {
            return first == InID ? second : first;
        }
    };
}
