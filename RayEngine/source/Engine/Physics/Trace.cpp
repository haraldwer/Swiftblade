#include "Trace.h"

#include <PxQueryReport.h>
#include <PxScene.h>
#include <PxShape.h>

#include "Manager.h"
#include "Utility/PhysXUtility.h"

Physics::QueryResult Physics::Query::Trace(const TraceParams& InParams)
{
    Vec3F diff = (InParams.End - InParams.Start);
    Vec3F dir = diff.GetNormalized();
    float length = diff.Length();
    
    QueryResult result;

    constexpr size_t numHits = 8;
    physx::PxRaycastHit hits[numHits];
    physx::PxRaycastBuffer buff(hits, numHits);

    // TODO: Filtering
    
    CHECK_ASSERT(!Manager::Get().Scene, "Invalid scene"); 
    Manager::Get().Scene->raycast(
        Utility::PhysX::ConvertVec(InParams.Start),
        Utility::PhysX::ConvertVec(dir),
        length,
        buff);

    if (buff.nbTouches > 0)
    {
        result.IsHit = true;
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
        }
    }

    return result;
}

Physics::QueryResult Physics::Query::Sweep(const SweepParams& InParams)
{
    Vec3F diff = (InParams.End - InParams.Start);
    Vec3F dir = diff.GetNormalized();
    float length = diff.Length();
    
    QueryResult result;
    
    physx::PxGeometry* geometry = Manager::GetGeometry(InParams.Shape, InParams.ShapeData);
    CHECK_RETURN(!geometry, result);
    
    constexpr size_t numHits = 8;
    physx::PxSweepHit hits[numHits];
    physx::PxSweepBuffer buff(hits, numHits);

    const physx::PxTransform pose = physx::PxTransform(
        Utility::PhysX::ConvertVec(InParams.Start + InParams.Pose.GetPosition()),
        Utility::PhysX::ConvertQuat(InParams.Pose.GetRotation()));
    
    // TODO: Filtering
    CHECK_ASSERT(!Manager::Get().Scene, "Invalid scene"); 
    Manager::Get().Scene->sweep(
        *geometry,
        pose,
        Utility::PhysX::ConvertVec(dir),
        length,
        buff);
    
    if (buff.nbTouches > 0)
    {
        result.IsHit = true;
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
        }
    }

    return result;
}
