#pragma once

#include "CollisionShape.h"
#include "Engine/ECS/Entity.h"

namespace Physics
{
    struct QueryResult
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

    struct TraceParams
    {
        Vec3F Start; 
        Vec3F End; 
    };

    struct SweepParams
    {
        Vec3F Start;
        Vec3F End;
        Shape Shape;
        Vec4F ShapeData;
        Mat4F Pose;
    }; 
    
    class Query
    {
        friend class Manager;
    public:
        static QueryResult Trace(const TraceParams& InParams);
        static QueryResult Sweep(const SweepParams& InParams);
    };
}
