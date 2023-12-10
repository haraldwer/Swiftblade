#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

struct SectionEnd : ECS::Component<SectionEnd>
{
};

class SysSectionEnd : public ECS::System<SectionEnd>
{
};
