#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct GameEnd : ECS::Component<GameEnd>
{
};

class SysGameEnd : public ECS::System<GameEnd>
{
};
