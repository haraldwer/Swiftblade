#include "RoomGenPath.h"

#include "ECS/Volume/CubeVolume.h"

bool RoomGenPath::Step()
{
    ECS::VolumeCoord coord = path.empty() ? start : path.back();
    if (coord.key == target.key)
        return true;

    const Vec3F startPos = Vec3F(start.pos.x, start.pos.y, start.pos.z);
    const Vec3F targetPos = Vec3F(target.pos.x, target.pos.y, target.pos.z);
    const Vec3F basePos = Vec3F(coord.pos.x, coord.pos.y, coord.pos.z);
    
    Vec3F lastDir = Vec3F::Forward();
    if (path.size() > 1)
    {
        ECS::VolumeCoord lastCoord = path[path.size() - 2];
        Vec3F lastPos = Vec3F(lastCoord.pos.x, lastCoord.pos.y, lastCoord.pos.z);
        lastDir = (basePos - lastPos).GetNormalized();
    }

    // Collect in random pool
    Utility::RandomWeightedCollection<ECS::VolumeCoord> pool(seed + static_cast<int>(path.size()));
    
    // Add every direction
    for (ECS::VolumeCoord direction : ECS::CubeVolume::GetNeighbors(coord))
        if (direction.key != 0 && direction.pos.z <= target.pos.z)
            EvaluateDirection(direction, targetPos, startPos, basePos, lastDir, pool);

    if (pool.Count() == 0)
    {
        path.push_back(target.key);
        pathSet.insert(target.key);
        return true;
    }
    
    ECS::VolumeCoord result = pool.Pop();
    CHECK_RETURN(pathSet.contains(result.key), false);
    path.push_back(result.key);
    pathSet.insert(result.key);

    sparseCountdown--;
    if (sparseCountdown <= 0 && result.key != target.key)
    {
        Random rnd(seed + static_cast<int>(path.size()));
        sparseCountdown = static_cast<int>(rnd.Range(minSparseStep, maxSparseStep));
        sparsePath.push_back(result.key);
    }
    
    return result.key == target.key;
}

void RoomGenPath::EvaluateDirection(const ECS::VolumeCoord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<ECS::VolumeCoord>& InOutPool) const
{
    if (pathSet.contains(InNewCoord.key))
        return;
        
    // Distance
    Vec3F coordPos = Vec3F(InNewCoord.pos.x, InNewCoord.pos.y, InNewCoord.pos.z);
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