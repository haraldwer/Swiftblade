#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct RoomConnection : ECS::Component<RoomConnection>
{
    PROPERTY_D(bool, IsEnd, true); 
};

class SysRoomConnection : public ECS::System<RoomConnection>
{
};
