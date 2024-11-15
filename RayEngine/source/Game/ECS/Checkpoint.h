#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct Checkpoint : ECS::Component<Checkpoint>
{
};

class SysCheckpoint : public ECS::System<Checkpoint>
{
};
