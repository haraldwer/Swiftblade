#pragma once

#include "Utility.h"
#include "QueryStructs.h"
#include "reactphysics3d/collision/CollisionCallback.h"
#include "reactphysics3d/collision/RaycastInfo.h"

namespace Physics
{
    class TraceCallback : public reactphysics3d::RaycastCallback
    {
    public:
        TraceCallback(const TraceParams& InParams)
        {
            totalDistance = (InParams.end - InParams.start).Length();
        }

        QueryResult GetResult() const { return result; }

    private:
        reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& InInfo) override
        {
            auto id = reinterpret_cast<ECS::EntityID>(InInfo.body->getUserData());
            if (params.ignoredEntities.contains(id))
                return 1;
            
            result.isHit = true;
            auto& hit = result.hits.emplace_back();
            hit.position = GetVec(InInfo.worldPoint);
            hit.normal = GetVec(InInfo.worldNormal);
            hit.distance = totalDistance * InInfo.hitFraction;
            hit.entity = id;
            return static_cast<int>(result.hits.size()) >= params.maxHits ? 0 : 1;
        }

        QueryResult result;
        TraceParams params;
        float totalDistance = 0.0f;
    };

    class SweepCallback : public reactphysics3d::CollisionCallback
    {
    private:
        void onContact(const CallbackData &callbackData) override
        {
            
        }
    };
}
