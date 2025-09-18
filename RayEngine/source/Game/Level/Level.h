#pragma once
#include "Engine/Scene/SceneResource.h"
#include "Database/NetResource.h"

struct Level : PropertyOwner<Level>
{
    PROPERTY(String, Name);
    PROPERTY(String, Creator);
    PROPERTY(String, LastEdit);
    
    PROPERTY(ResScene, SectionStart)
    PROPERTY(ResScene, SectionEnd);
    PROPERTY(ResScene, GameStart);
    PROPERTY(ResScene, GameEnd);

    // Number of rooms per section
    PROPERTY(int, NumRooms);
    PROPERTY(Vector<ResScene>, Rooms);

    // Number of arenas AKA number of sections
    PROPERTY(int, NumArenas);
    PROPERTY(Vector<ResScene>, Arenas);
};

typedef Resource::PropertyFile<Level> LevelResource;
typedef Resource::Ref<LevelResource> ResLevel;