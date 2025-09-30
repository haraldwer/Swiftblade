#pragma once
#include "Game/ECS/Volume/CubeVolumeCoord.h"
#include "Game/ECS/Volume/CubeVolumeData.h"
#include "Game/Level/RoomInfo.h"
#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

struct EditRoomObject : PropertyOwner<EditRoomObject>
{
    PROPERTY(ECS::VolumeCoordKey, Coord);
    PROPERTY(String, Object); // Object type, maps to a blueprint
    PROPERTY(int, Rotations);
};

typedef Map<ECS::VolumeCoordKey, EditRoomObject> EditRoomObjectMap; 

// A room representation for the editor.
// This room will be converted to a scene before being played.
struct EditRoom : PropertyOwner<EditRoom>
{
    PROPERTY(RoomInfo, Info);
    
    PROPERTY_D(ECS::VolumeCoordKey, Connection, 0)
    PROPERTY(Vector<ECS::VolumeCoordKey>, Path);
    PROPERTY(EditRoomObjectMap, Objects);
    
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

typedef Resource::PropertyFile<EditRoom> EditRoomResource;
typedef Resource::Ref<EditRoomResource> ResEditRoom;