#pragma once
#include "Engine/Scene/SceneResource.h"
#include "Resource/PropertyFile.h"
#include "RoomInfo.h"

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
    PROPERTY(Vector<RoomInfo>, Rooms);
    PROPERTY(int, NumArenas);
    PROPERTY(Vector<RoomInfo>, Arenas);

    static bool Accept(const String& InPath)
    {
        return InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("L_");
    }
};

typedef Resource::PropertyFile<Level> LevelResource;
typedef Resource::Ref<LevelResource> ResLevel;