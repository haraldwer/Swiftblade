#pragma once

#include "Resource/PropertyFile.h"
#include "Resource/Resource.h"

enum class RoomType : uint8
{
    ROOM = 0,
    ARENA,
    START,
    END
};

inline RoomType FromStr(const String& InStr)
{
    if (InStr == "ARENA") return RoomType::ARENA;
    if (InStr == "START") return RoomType::START;
    if (InStr == "END") return RoomType::END;
    return RoomType::ROOM;
}

inline String ToStr(const RoomType InType)
{
    switch (InType)
    {
        case RoomType::ARENA: return "ARENA";
        case RoomType::START: return "START";
        case RoomType::END: return "END";
    }
    return "ROOM";
}

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
    PROPERTY(int, Type);
};

typedef Resource::PropertyFile<RoomInfo> RoomResource;
typedef Resource::Ref<RoomResource> ResRoom;