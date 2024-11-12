#pragma once

#include "RoomGenBase.h"
#include "Game/ECS/Volume/Coord.h"

class RoomGenVolume : public RoomGenBase
{
public:
    void Clear() override;
    void Init() override;
    bool Step() override;

private:

    void TryQueueEntry(Coord InNewCoord, Coord InReference);
    bool EvaluateCoord(Coord InCoord, Coord InReference, uint8& InOutValue);
    
    struct QueuedEntry
    {
        uint32 Coord = 0;
        uint32 Ref = 0;
    };
    
    Vector<QueuedEntry> QueuedCoords;
    Map<uint32, uint32> NextQueue;
    Map<uint32, Set<uint32>> CheckedCoords;
    Map<uint32, uint8> Result;
    
    int VolumeDepth = 0; 
};
