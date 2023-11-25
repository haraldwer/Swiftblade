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
            ECS::EntityID Entity = ECS::InvalidID;
        };
        Vector<Hit> Hits; 
        bool IsHit = false;
        Hit ClosestHit() const;
        Vector<Hit> DistanceSorted() const;
    };

    struct TraceParams
    {
        Vec3F Start; 
        Vec3F End;
        Set<ECS::EntityID> IgnoredEntities; 
    };

    struct SweepParams
    {
        Vec3F Start;
        Vec3F End;
        Shape Shape;
        Vec4F ShapeData;
        Mat4F Pose;
        Set<ECS::EntityID> IgnoredEntities; 
    }; 
    
    class Query
    {
        friend class Manager;
    public:
        static QueryResult Trace(const TraceParams& InParams);
        static QueryResult Sweep(const SweepParams& InParams);
    };
}
