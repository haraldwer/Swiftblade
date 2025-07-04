﻿#pragma once

#include "ECS/Volume/Coord.h"
#include "RoomGenBase.h"

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
        uint32 coord = 0;
        uint32 ref = 0;
    };
    
    Vector<QueuedEntry> queuedCoords = {};
    Map<uint32, uint32> nextQueue = {};
    Map<uint32, Set<uint32>> checkedCoords = {};
    Map<uint32, uint8> result = {};
    
    int volumeDepth = 0; 
};
