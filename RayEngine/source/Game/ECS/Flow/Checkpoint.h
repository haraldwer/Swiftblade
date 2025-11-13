#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct Checkpoint : Component<Checkpoint>
    {
    };

    class SysCheckpoint : public System<Checkpoint>
    {
    };
}
