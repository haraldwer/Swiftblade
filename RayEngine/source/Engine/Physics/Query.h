    #pragma once

#include "CollisionShape.h"
#include "ECS/Entity.h"

namespace Physics
{
    struct QueryResult
    {
        struct Hit
        {
            float distance = 0.0f;
            Vec3F position = {};
            Vec3F normal = {};
            ECS::EntityID entity = ECS::INVALID_ID;
        };
        Vector<Hit> hits = {}; 
        bool isHit = false;
        Hit ClosestHit() const;
        Vector<Hit> DistanceSorted() const;
    };

    struct TraceParams
    {
        Vec3F start = {}; 
        Vec3F end = {};
        Set<ECS::EntityID> ignoredEntities = {}; 
    };

    struct SweepParams
    {
        Vec3F start = {};
        Vec3F end = {};
        Shape shape = {};
        Vec4F shapeData = {};
        Mat4F pose = {};
        Set<ECS::EntityID> ignoredEntities = {}; 
    }; 
    
    class Query
    {
        friend class Manager;
    public:
        static QueryResult Trace(const TraceParams& InParams);
        static QueryResult Sweep(const SweepParams& InParams);
    };
}
