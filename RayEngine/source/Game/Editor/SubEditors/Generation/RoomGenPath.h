#pragma once

#include "Utility/Collections/RandomWeightedCollection.h"
#include "RoomGenBase.h"
#include "ECS/Volume/CubeVolumeCoord.h"

class RoomGenPath : public RoomGenBase
{
public:
    void Init() override;
    bool Step() override;
    void Clear() override;

    const Vector<ECS::VolumeCoord>& GetPath() const { return path; }
    
private:

    void EvaluateDirection(ECS::VolumeCoord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<ECS::VolumeCoord>& InOutPool) const;

    Vector<ECS::VolumeCoord> path = {};
    Set<uint32> pathSet = {};
    ECS::VolumeCoord start = {};
    ECS::VolumeCoord target = {};
};
