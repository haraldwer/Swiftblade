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

    const Vector<Coord>& GetPath() const { return Path; }
    
private:

    void EvaluateDirection(Coord InNewCoord, const Vec3F& InTargetPos, const Vec3F& InStartPos, const Vec3F& InBasePos, const Vec3F& InLastDir, Utility::RandomWeightedCollection<Coord>& InOutPool) const;

    Vector<Coord> Path = {};
    Set<uint32> PathSet = {};
    Coord Start = {};
    Coord Target = {};
};
