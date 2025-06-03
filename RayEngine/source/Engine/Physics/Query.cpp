#include "Query.h"

#include <algorithm>
#include <PxQueryReport.h>
#include <PxScene.h>
#include <PxShape.h>

#include "Manager.h"
#include "QueryFilter.h"
#include "Editor/Debug/Draw.h"
#include "PhysXUtility.h"

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
    Vector<Hit> hits = hits;
    std::ranges::sort(hits.begin(), hits.end(), [](const Hit& InFirst, const Hit& InSecond)
    {
        return InFirst.distance < InSecond.distance; 
    });
    return hits;
}

Physics::QueryResult Physics::Query::Trace(const TraceParams& InParams)
{
    Vec3F diff = (InParams.end - InParams.start);
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
    filter.SetIgnoredEntities(InParams.ignoredEntities);
    physx::PxQueryFilterCallback* filterCallback = &filter;
    
    CHECK_ASSERT(!Manager::Get().scene, "Invalid scene"); 
    Manager::Get().scene->raycast(
        Utility::PhysX::ConvertVec(InParams.start),
        Utility::PhysX::ConvertVec(dir),
        length,
        buff,
        flags,
        filterData,
        filterCallback);

    Rendering::DebugLine(InParams.start, InParams.end);
    
    for (int i = 0; i < static_cast<int>(buff.nbTouches); i++)
    {
        auto& touch = buff.touches[i];

        if (!(static_cast<uint8>(touch.shape->getFlags()) &
            static_cast<uint8>(physx::PxShapeFlag::eSCENE_QUERY_SHAPE)))
                continue;
        
        auto& hit = result.hits.emplace_back(); 
        hit.distance = touch.distance;
        hit.position = Utility::PhysX::ConvertVec(touch.position);
        hit.normal = Utility::PhysX::ConvertVec(touch.normal);
        hit.entity = ECS::PtrToEntity(touch.shape->userData);
        result.hits.push_back(hit);
        result.isHit = true;
    }
    return result;
}

Physics::QueryResult Physics::Query::Sweep(const SweepParams& InParams)
{
    const Vec3F diff = (InParams.end - InParams.start);
    const Vec3F dir = diff.GetNormalized();
    const float length = diff.Length();
    
    QueryResult result;
    
    const physx::PxGeometry* geometry = Manager::GetGeometry(InParams.shape, InParams.shapeData, InParams.pose.GetScale());
    CHECK_RETURN(!geometry, result);

    const physx::PxTransform pose = physx::PxTransform(
        Utility::PhysX::ConvertVec(InParams.start + InParams.pose.GetPosition()),
        Utility::PhysX::ConvertQuat(InParams.pose.GetRotation()));
    
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
    filter.SetIgnoredEntities(InParams.ignoredEntities);
    physx::PxQueryFilterCallback* filterCallback = &filter;
    
    // TODO: Filtering
    auto scene = Manager::Get().scene;
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

    result.isHit = buff.nbTouches > 0;
    result.hits.reserve(buff.nbTouches);
    for (int i = 0; i < static_cast<int>(buff.nbTouches); i++)
    {
        const physx::PxSweepHit& touch = buff.touches[i];
        
        if (!(static_cast<uint8>(touch.shape->getFlags()) &
            static_cast<uint8>(physx::PxShapeFlag::eSCENE_QUERY_SHAPE)))
                continue;

        QueryResult::Hit& hit = result.hits.emplace_back(); 
        hit.distance = touch.distance;
        hit.position = Utility::PhysX::ConvertVec(touch.position);
        hit.normal = Utility::PhysX::ConvertVec(touch.normal);
        hit.entity = ECS::PtrToEntity(touch.shape->userData);
    }

    return result;
}
