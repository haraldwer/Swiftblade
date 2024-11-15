#pragma once

#include "Blueprints/Blueprint.h"
#include "ECS/System.h"
#include "ECS/Component.h"

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
