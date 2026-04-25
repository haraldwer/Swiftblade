#pragma once
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"

namespace ECS
{
    struct SectionEnd : Component<SectionEnd>
    {
    };

    class SysSectionEnd : public System<SectionEnd>
    {
    };
}
