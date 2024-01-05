#pragma once

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

namespace ECS
{
    struct Attributes : Component<Attributes>
    {
        PROPERTY(String, Name);
        ResBlueprint Blueprint; 
    };

    class SysAttributes : public System<Attributes>
    {
    public:
        void Init(EntityID InID, Attributes& InComponent) override;
    };
}
