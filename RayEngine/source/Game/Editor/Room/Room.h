#pragma once
#include "Game/ECS/Volume/CubeVolumeCoord.h"
#include "Game/ECS/Volume/CubeVolumeData.h"

struct RoomObject : PropertyOwner<RoomObject>
{
    PROPERTY(ECS::VolumeCoordKey, Coord);
    PROPERTY(String, Object); // Object type, maps to a blueprint
    PROPERTY(int, Rotations);
};

typedef Map<ECS::VolumeCoordKey, RoomObject> RoomObjectMap; 

// A room representation for the editor.
// This room will be converted to a scene before being played.
struct Room : PropertyOwner<Room>
{
    PROPERTY(String, Name);
        
    PROPERTY_D(ECS::VolumeCoordKey, Connection, 0)
    PROPERTY(Vector<ECS::VolumeCoordKey>, Path);
    PROPERTY(RoomObjectMap, Objects);
    
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
