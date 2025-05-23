﻿#pragma once
#include "Engine/Scene/SceneResource.h"

struct RoomConfig : PropertyOwner<RoomConfig>
{
    PROPERTY(ResScene, SectionStart);
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
