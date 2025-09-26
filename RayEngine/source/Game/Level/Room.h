#pragma once

#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

struct RoomEntry : PropertyOwner<RoomEntry>
{
    PROPERTY(String, ID);
    PROPERTY(String, Name);
    PROPERTY(String, Creator);
};

struct Room : PropertyOwner<Room>
{
    PROPERTY(String, Name);
    PROPERTY(String, Creator);
    PROPERTY(String, Scene);
    PROPERTY(Vec3I, Size);
    PROPERTY(float, Length);
    PROPERTY(int, Spawners);
    PROPERTY(int, Objects);
};

typedef Resource::PropertyFile<Room> RoomResource;
typedef Resource::Ref<RoomResource> ResRoom;