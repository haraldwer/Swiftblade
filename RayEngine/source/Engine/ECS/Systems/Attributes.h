#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Attributes : Component<Attributes>
    {
        PROPERTY(String, Name);
    };

    class SysAttributes : public System<Attributes>
    {
    public:
        void Init(EntityID InID, Attributes& InComponent) override;
    };
}
