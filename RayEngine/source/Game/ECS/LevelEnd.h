#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct LevelEnd : ECS::Component<LevelEnd>
{
};

class SysLevelEnd : public ECS::System<LevelEnd>
{
};
