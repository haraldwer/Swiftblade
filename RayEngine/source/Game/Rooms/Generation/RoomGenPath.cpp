#include "RoomGenPath.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/SubEditors/RoomGenEditor.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"

void RoomGenPath::Clear()
{
    path.clear();
    pathSet.clear();
    start = Coord();
    target = Coord(); 
}

void RoomGenPath::Init()
{
    CHECK_RETURN(!owner);
    
    // Cache start and end position
    const ECS::CubeVolume& v = owner->GetVolume();
    if (const auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(owner->startEntity))
        start = v.PosToCoord(t->GetPosition() + Vec3F(1, 1, -1) * 0.6f);
    if (const auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(owner->endEntity))
        target = v.PosToCoord(t->GetPosition() + Vec3F(1, 1, -1) * 0.6f);
}

bool RoomGenPath::Step()
{
    CHECK_RETURN(!owner, true);
    
    Coord coord = path.empty() ? start : path.back();
    if (coord.Key == target.Key)
        return true;

    const Vec3F startPos = Vec3F(start.Pos.X, start.Pos.Y, start.Pos.Z);
    const Vec3F targetPos = Vec3F(target.Pos.X, target.Pos.Y, target.Pos.Z);
    const Vec3F basePos = Vec3F(coord.Pos.X, coord.Pos.Y, coord.Pos.Z);
    
    Vec3F lastDir = Vec3F::Forward();
    if (path.size() > 1)
    {
        Coord lastCoord = path[path.size() - 2];
        Vec3F lastPos = Vec3F(lastCoord.Pos.X, lastCoord.Pos.Y, lastCoord.Pos.Z);
        lastDir = (basePos - lastPos).GetNormalized();
    }

    // Collect in random pool
    Utility::RandomWeightedCollection<Coord> pool(owner->seed + static_cast<int>(path.size()));
    
    // Add every direction
    for (Coord direction : ECS::CubeVolume::GetNeighbors(coord))
        if (direction.Key != 0)
            EvaluateDirection(direction, targetPos, startPos, basePos, lastDir, pool);

    if (pool.Count() == 0)
    {
        path.push_back(target);
        pathSet.insert(target.Key);
        return true;
    }
    
    Coord result = pool.Pop();
    CHECK_RETURN(pathSet.contains(result.Key), false);
    path.push_back(result);
    pathSet.insert(result.Key);
    return result.Key == target.Key;
}

void RoomGenPath::EvaluateDirection(const Coord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<Coord>& InOutPool) const
{
    if (pathSet.contains(InNewCoord.Key))
        return;
        
    // Distance
    Vec3F coordPos = Vec3F(InNewCoord.Pos.X, InNewCoord.Pos.Y, InNewCoord.Pos.Z);
    float dist = (InTargetPos - coordPos).Length(); 
    float totalDist = (InTargetPos - InStartPos).Length();
    float distPart = Utility::Math::Min(1.0f, dist / totalDist);

    // Dot 
    Vec3F dir = (coordPos - InBasePos).GetNormalized();
    Vec3F targetDir = (InTargetPos - coordPos).GetNormalized();
    float dot = Vec3F::Dot(dir, targetDir);

    // Last dot
    float lastDot = Utility::Math::Lerp(1.0f, Vec3F::Dot(InLastDir, dir), 0.7f);

    // Randomness factor
    float randomness = 0.6f; 

    // Only change direction ever so often...
    float weight = (dot + randomness * distPart) * lastDot;
    if (weight < 0.001f)
        return;
        
    InOutPool.Add(InNewCoord, weight);
};