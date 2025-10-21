#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct GameEnd : Component<GameEnd>
    {
    };

    class SysGameEnd : public System<GameEnd>
    {
    };
}
