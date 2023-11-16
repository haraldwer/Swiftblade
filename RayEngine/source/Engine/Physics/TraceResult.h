#pragma once

#include "Engine/ECS/entity.h"

namespace Physics
{
    struct TraceResult
    {
        struct Hit
        {
            float Distance = 0.0f;
            Vec3F Position;
            Vec3F Normal;
            ECS::EntityID Entity;
        };
        Vector<Hit> Hits; 
        bool IsHit;
    };
}
