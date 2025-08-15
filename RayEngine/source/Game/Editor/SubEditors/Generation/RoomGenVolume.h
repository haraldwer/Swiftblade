#pragma once

#include "ECS/Volume/CubeVolumeCoord.h"

namespace ECS
{
    struct CubeVolumeData;
}

class RoomGenVolume
{
public:
    RoomGenVolume() = default;
    RoomGenVolume(const Vector<ECS::VolumeCoordKey>& InPath);
    bool Step(ECS::CubeVolumeData& InOutVolume);

private:

    void TryQueueEntry(ECS::VolumeCoord InNewCoord, ECS::VolumeCoord InReference);
    bool EvaluateCoord(ECS::VolumeCoord InCoord, ECS::VolumeCoord InReference, uint8& InOutValue);
    
    struct QueuedEntry
    {
        ECS::VolumeCoordKey coord = 0;
        ECS::VolumeCoordKey ref = 0;
    };
    
    Vector<ECS::VolumeCoordKey> path = {};
    Vector<QueuedEntry> queuedCoords = {};
    Map<ECS::VolumeCoordKey, ECS::VolumeCoordKey> nextQueue = {};
    Map<ECS::VolumeCoordKey, Set<ECS::VolumeCoordKey>> checkedCoords = {};
    Map<ECS::VolumeCoordKey, ECS::VolumeDataType> result = {};
    
    int volumeDepth = 0;
    static const int stepSize = 200;
};
