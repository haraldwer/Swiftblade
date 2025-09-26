#pragma once
#include "Engine/Scene/SceneResource.h"
#include "Resource/PropertyFile.h"
#include "Room.h"

// The actual level being played
struct LevelConfig : PropertyOwner<LevelConfig>
{
    PROPERTY(int, Seed);
    PROPERTY(int, ProppingSeed);
    
    PROPERTY_D(ResScene, SectionStart, "Scenes/S_GameStart.json")
    PROPERTY_D(ResScene, SectionEnd, "Scenes/S_GameEnd.json");
    PROPERTY_D(ResScene, GameStart, "Scenes/S_SectionStart.json");
    PROPERTY_D(ResScene, GameEnd, "Scenes/S_SectionEnd.json");

    // Number of rooms per section
    PROPERTY(int, NumRooms);
    PROPERTY(Vector<ResScene>, Rooms);

    // Number of arenas AKA number of sections
    PROPERTY(int, NumArenas);
    PROPERTY(Vector<ResScene>, Arenas);
};

// The level being passed around in the backend
struct Level : PropertyOwner<Level>
{
    PROPERTY(String, Name);
    PROPERTY(String, Creator);
    PROPERTY(String, LastEdit);

    PROPERTY(int, NumRooms);
    PROPERTY(Vector<RoomEntry>, Rooms);
    PROPERTY(int, NumArenas);
    PROPERTY(Vector<RoomEntry>, Arenas);
};

typedef Resource::PropertyFile<Level> LevelResource;
typedef Resource::Ref<LevelResource> ResLevel;