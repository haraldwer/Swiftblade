#pragma once

#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

struct RoomData : PropertyOwner<RoomData>
{
    PROPERTY(int, Playcount);
    PROPERTY(int, Favourites);
    PROPERTY(String, Status);
};

struct RoomInfo : PropertyOwner<RoomInfo>
{
    PROPERTY(String, ID);
    PROPERTY(String, Name);
    PROPERTY(String, CreatorID);
    PROPERTY(String, CreatorName);
    PROPERTY(float, Length);
    PROPERTY(Vec3I, Size);
    PROPERTY(int, Spawners);
    PROPERTY(int, Objects);
    PROPERTY(int, Rank);
    PROPERTY(int, Score);
};

typedef Resource::PropertyFile<RoomInfo> RoomResource;
typedef Resource::Ref<RoomResource> ResRoom;