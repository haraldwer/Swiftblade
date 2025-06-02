#pragma once

#include "ECS/Volume/Coord.h"
#include "Utility/Collections/RandomWeightedCollection.h"
#include "RoomGenBase.h"

class RoomGenPath : public RoomGenBase
{
public:
    void Init() override;
    bool Step() override;
    void Clear() override;

    const Vector<Coord>& GetPath() const { return path; }
    
private:

    void EvaluateDirection(Coord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<Coord>& InOutPool) const;

    Vector<Coord> path = {};
    Set<uint32> pathSet = {};
    Coord start = {};
    Coord target = {};
};
