#include "RoomGenPath.h"

#include "ECS/Volume/CubeVolume.h"
#include "Editor/SubEditors/RoomGenEditor.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"

void RoomGenPath::Clear()
{
    Path.clear();
    PathSet.clear();
    Start = Coord();
    Target = Coord(); 
}

void RoomGenPath::Init()
{
    CHECK_RETURN(!Owner);
    
    // Cache start and end position
    const ECS::CubeVolume& v = Owner->GetVolume();
    if (auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(Owner->StartEntity))
        Start = v.PosToCoord(t->GetPosition() + Vec3F(1, 1, -1) * 0.6f);
    if (auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(Owner->EndEntity))
        Target = v.PosToCoord(t->GetPosition() + Vec3F(1, 1, -1) * 0.6f);
}

bool RoomGenPath::Step()
{
    CHECK_RETURN(!Owner, true);
    
    Coord coord = Path.empty() ? Start : Path.back();
    if (coord.Key == Target.Key)
        return true;

    Vec3F startPos = Vec3F(Start.Pos.X, Start.Pos.Y, Start.Pos.Z);
    Vec3F targetPos = Vec3F(Target.Pos.X, Target.Pos.Y, Target.Pos.Z);
    Vec3F basePos = Vec3F(coord.Pos.X, coord.Pos.Y, coord.Pos.Z);
    
    Vec3F lastDir = Vec3F::Forward();
    if (Path.size() > 1)
    {
        Coord lastCoord = Path[Path.size() - 2];
        Vec3F lastPos = Vec3F(lastCoord.Pos.X, lastCoord.Pos.Y, lastCoord.Pos.Z);
        lastDir = (basePos - lastPos).GetNormalized();
    }

    // Collect in random pool
    Utility::RandomWeightedCollection<Coord> pool(Owner->Seed + static_cast<int>(Path.size()));
    
    // Add every direction
    for (Coord direction : ECS::CubeVolume::GetNeighbors(coord))
        if (direction.Key != 0)
            EvaluateDirection(direction, targetPos, startPos, basePos, lastDir, pool);

    if (pool.Count() == 0)
    {
        Path.push_back(Target);
        PathSet.insert(Target.Key);
        return true;
    }
    
    Coord result = pool.Pop();
    CHECK_RETURN(PathSet.contains(result.Key), false);
    Path.push_back(result);
    PathSet.insert(result.Key);
    return result.Key == Target.Key;
}

void RoomGenPath::EvaluateDirection(const Coord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<Coord>& InOutPool) const
{
    if (PathSet.contains(InNewCoord.Key))
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