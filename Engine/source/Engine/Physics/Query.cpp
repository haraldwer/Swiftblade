#include "Query.h"

#include "ContactHandler.h"
#include "Manager.h"
#include "QueryCallback.h"
#include "Utility.h"
#include "Editor/Debug/Draw.h"
#include "reactphysics3d/engine/PhysicsWorld.h"

Physics::QueryResult Physics::Query::Trace(const TraceParams& InParams)
{
    auto& man = Manager::Get();
    CHECK_ASSERT(!man.world, "Invalid world");
    auto ray = reactphysics3d::Ray(GetVec(InParams.start), GetVec(InParams.end), InParams.radius);
    auto callback = TraceCallback(InParams);
    man.world->raycast(ray, &callback);
    return callback.GetResult();
}

Physics::QueryResult Physics::Query::Sweep(const SweepParams& InParams)
{
    auto& man = Manager::Get();
    CHECK_ASSERT(!man.world, "Invalid world");
    
    TraceParams trace;
    trace.start = InParams.start;
    trace.end = InParams.end;
    trace.radius = 1.0f;
    trace.maxHits = InParams.maxHits;
    trace.ignoredEntities = InParams.ignoredEntities;
    return Trace(trace);
}

Vector<Physics::Contact> Physics::Query::GetContacts(ECS::EntityID InID)
{
    auto& man = Manager::Get();
    CHECK_ASSERT(!man.world, "Invalid contact handler");
    return man.contactHandler->GetContacts(InID);
}
