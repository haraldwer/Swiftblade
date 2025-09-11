#include "Query.h"

#include <algorithm>
#include "Editor/Debug/Draw.h"

Physics::QueryResult::Hit Physics::QueryResult::ClosestHit() const
{
    Hit result;
    result.distance = -1.0f; 
    for (auto& hit : hits)
        if (hit.distance < result.distance || result.distance < 0.0)
            result = hit;
    return result; 
}

Vector<Physics::QueryResult::Hit> Physics::QueryResult::DistanceSorted() const
{
    Vector<Hit> result = hits;
    std::ranges::sort(result.begin(), result.end(), [](const Hit& InFirst, const Hit& InSecond)
    {
        return InFirst.distance < InSecond.distance; 
    });
    return result;
}

Physics::QueryResult Physics::Query::Trace(const TraceParams& InParams)
{
    Vec3F diff = (InParams.end - InParams.start);
    Vec3F dir = diff.GetNormalized();
    float length = diff.Length();
    
    QueryResult result;

    Rendering::DebugLine(InParams.start, InParams.end);
    
    return result;
}

Physics::QueryResult Physics::Query::Sweep(const SweepParams& InParams)
{
    const Vec3F diff = (InParams.end - InParams.start);
    const Vec3F dir = diff.GetNormalized();
    const float length = diff.Length();
    
    QueryResult result;
    
    return result;
}
