#include "Query.h"

#include <algorithm>
#include <PxQueryReport.h>
#include <PxScene.h>
#include <PxShape.h>

#include "Manager.h"
#include "QueryFilter.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Rendering/Manager.h"
#include "Engine/Rendering/Debug/Draw.h"
#include "Utility/PhysXUtility.h"

Physics::QueryResult::Hit Physics::QueryResult::ClosestHit() const
{
    Hit result;
    result.Distance = -1.0f; 
    for (auto& hit : Hits)
        if (hit.Distance < result.Distance || result.Distance < 0.0)
            result = hit;
    return result; 
}

Vector<Physics::QueryResult::Hit> Physics::QueryResult::DistanceSorted() const
{
    Vector<Hit> hits = Hits;
    std::ranges::sort(hits.begin(), hits.end(), [](const Hit& InFirst, const Hit& InSecond)
    {
        return InFirst.Distance < InSecond.Distance; 
    });
    return hits;
}

Physics::QueryResult Physics::Query::Trace(const TraceParams& InParams)
{
    Vec3F diff = (InParams.End - InParams.Start);
    Vec3F dir = diff.GetNormalized();
    float length = diff.Length();
    
    QueryResult result;

    constexpr size_t numHits = 8;
    physx::PxRaycastHit hits[numHits];
    physx::PxRaycastBuffer buff(hits, numHits);

    physx::PxHitFlags flags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
    physx::PxQueryFilterData filterData;
    filterData.flags |= physx::PxQueryFlag::ePREFILTER; // Enable filter
    static QueryFilter filter;
    filter.SetIgnoredEntities(InParams.IgnoredEntities);
    physx::PxQueryFilterCallback* filterCallback = &filter;
    
    CHECK_ASSERT(!Manager::Get().Scene, "Invalid scene"); 
    Manager::Get().Scene->raycast(
        Utility::PhysX::ConvertVec(InParams.Start),
        Utility::PhysX::ConvertVec(dir),
        length,
        buff,
        flags,
        filterData,
        filterCallback);

    Rendering::DebugLine(InParams.Start, InParams.End);
    
    for (int i = 0; i < static_cast<int>(buff.nbTouches); i++)
    {
        auto& touch = buff.touches[i];

        if (!(static_cast<uint8>(touch.shape->getFlags()) &
            static_cast<uint8>(physx::PxShapeFlag::eSCENE_QUERY_SHAPE)))
                continue;
        
        auto& hit = result.Hits.emplace_back(); 
        hit.Distance = touch.distance;
        hit.Position = Utility::PhysX::ConvertVec(touch.position);
        hit.Normal = Utility::PhysX::ConvertVec(touch.normal);
        hit.Entity = ECS::PtrToEntity(touch.shape->userData);
        result.Hits.push_back(hit);
        result.IsHit = true;
    }
    return result;
}

Physics::QueryResult Physics::Query::Sweep(const SweepParams& InParams)
{
    const Vec3F diff = (InParams.End - InParams.Start);
    const Vec3F dir = diff.GetNormalized();
    const float length = diff.Length();
    
    QueryResult result;
    
    const physx::PxGeometry* geometry = Manager::GetGeometry(InParams.Shape, InParams.ShapeData, InParams.Pose.GetScale());
    CHECK_RETURN(!geometry, result);

    const physx::PxTransform pose = physx::PxTransform(
        Utility::PhysX::ConvertVec(InParams.Start + InParams.Pose.GetPosition()),
        Utility::PhysX::ConvertQuat(InParams.Pose.GetRotation()));
    
    constexpr size_t numHits = 8;
    physx::PxSweepHit hits[numHits];
    physx::PxSweepBuffer buff(hits, numHits);
    
    const physx::PxHitFlags flags = physx::PxHitFlag::eDEFAULT;
    const physx::PxQueryFlags queryFlags = 
        physx::PxQueryFlag::eSTATIC |
        physx::PxQueryFlag::eDYNAMIC |
        physx::PxQueryFlag::ePREFILTER; // Enable filter 
    const physx::PxQueryFilterData filterData =
        physx::PxQueryFilterData(queryFlags);
    
    QueryFilter filter;
    filter.SetIgnoredEntities(InParams.IgnoredEntities);
    physx::PxQueryFilterCallback* filterCallback = &filter;
    
    // TODO: Filtering
    auto scene = Manager::Get().Scene;
    CHECK_ASSERT(!scene, "Invalid scene");
    scene->sweep(
        *geometry,
        pose,
        Utility::PhysX::ConvertVec(dir),
        length,
        buff,
        flags, 
        filterData,
        filterCallback);

    result.IsHit = buff.nbTouches > 0;
    result.Hits.reserve(buff.nbTouches);
    for (int i = 0; i < static_cast<int>(buff.nbTouches); i++)
    {
        const physx::PxSweepHit& touch = buff.touches[i];
        
        if (!(static_cast<uint8>(touch.shape->getFlags()) &
            static_cast<uint8>(physx::PxShapeFlag::eSCENE_QUERY_SHAPE)))
                continue;

        QueryResult::Hit& hit = result.Hits.emplace_back(); 
        hit.Distance = touch.distance;
        hit.Position = Utility::PhysX::ConvertVec(touch.position);
        hit.Normal = Utility::PhysX::ConvertVec(touch.normal);
        hit.Entity = ECS::PtrToEntity(touch.shape->userData);
    }

    return result;
}
