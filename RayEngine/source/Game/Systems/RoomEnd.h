#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct RoomEnd : ECS::Component<RoomEnd>
{
};

class SysRoomEnd : public ECS::System<RoomEnd>
{
};
