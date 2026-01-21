#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct RoomConnection : Component<RoomConnection>
    {
        PROPERTY_D(bool, IsEnd, true); 
    };

    class SysRoomConnection : public System<RoomConnection>
    {
    };
}
