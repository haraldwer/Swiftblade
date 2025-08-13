#pragma once

#include "ECS/Volume/CubeVolumeCoord.h"
#include "RoomGenBase.h"

class RoomGenVolume : public RoomGenBase
{
public:
    void Clear() override;
    void Init() override;
    bool Step() override;

private:

    void TryQueueEntry(ECS::VolumeCoord InNewCoord, ECS::VolumeCoord InReference);
    bool EvaluateCoord(ECS::VolumeCoord InCoord, ECS::VolumeCoord InReference, uint8& InOutValue);
    
    struct QueuedEntry
    {
        uint32 coord = 0;
        uint32 ref = 0;
    };
    
    Vector<QueuedEntry> queuedCoords = {};
    Map<uint32, uint32> nextQueue = {};
    Map<uint32, Set<uint32>> checkedCoords = {};
    Map<uint32, uint8> result = {};
    
    int volumeDepth = 0; 
};
