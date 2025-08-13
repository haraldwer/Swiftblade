#pragma once

#include "Utility/Collections/RandomWeightedCollection.h"
#include "ECS/Volume/CubeVolumeCoord.h"

class RoomGenPath
{
public:
    RoomGenPath(const ECS::VolumeCoord InStart, const ECS::VolumeCoord InTarget, const uint32 InSeed) :
        start(InStart), target(InTarget), seed(InSeed) {}
    
    bool Step();
    const Vector<ECS::VolumeCoord>& GetPath() const { return path; }

private:
    void EvaluateDirection(ECS::VolumeCoord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<ECS::VolumeCoord>& InOutPool) const;

    Vector<ECS::VolumeCoord> path = {};
    Set<ECS::VolumeCoord> pathSet = {};
    ECS::VolumeCoord start = {};
    ECS::VolumeCoord target = {};
    uint32 seed = 0;
};
