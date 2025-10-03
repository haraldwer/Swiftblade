#pragma once
#include "CollisionShape.h"
#include "ECS/Entity.h"

namespace Physics
{
    struct SweepParams
    {
        Vec3F start; 
        Vec3F end;
        Mat4F pose;
        Shape shape;
        Vec4F shapeData;
        int maxHits = 0;
        Set<ECS::EntityID> ignoredEntities;
    };
    
    struct TraceParams
    {
        Vec3F start; 
        Vec3F end;
        float radius = 0;
        int maxHits = 0;
        Set<ECS::EntityID> ignoredEntities;
    };
    
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
        
        Hit ClosestHit() const
        {
            Hit result;
            result.distance = -1.0f; 
            for (auto& hit : hits)
                if (hit.distance < result.distance || result.distance < 0.0)
                    result = hit;
            return result; 
        }
        
        Vector<Hit> DistanceSorted() const
        {
            Vector<Hit> result = hits;
            std::ranges::sort(result.begin(), result.end(), [](const Hit& InFirst, const Hit& InSecond)
            {
                return InFirst.distance < InSecond.distance; 
            });
            return result;
        }
    };
}