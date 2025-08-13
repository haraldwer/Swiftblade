#pragma once
#include "ECS/Volume/CubeVolumeCoord.h"
#include "ECS/Volume/CubeVolumeData.h"

struct RoomObject : PropertyOwner<RoomObject>
{
    PROPERTY(ECS::VolumeCoordKey, Coord);
    PROPERTY(String, Object); // Object type, maps to a blueprint
    PROPERTY(int, Rotations);
};

// A room representation for the editor.
// This room will be converted to a scene before being played.
struct Room : PropertyOwner<Room>
{
    PROPERTY(ECS::VolumeCoordKey, Connection);
    PROPERTY(Vector<ECS::VolumeCoordKey>, Path);
    PROPERTY(Vector<RoomObject>, Objects);
    
    ECS::CubeVolumeData volumeData;
    
    bool CustomDeserialize(const DeserializeObj &InObj) override
    {
        return volumeData.Deserialize(InObj);
    }
    
    void CustomSerialize(SerializeObj &InOutObj) const override
    {
        volumeData.Serialize(InOutObj);
    }
};