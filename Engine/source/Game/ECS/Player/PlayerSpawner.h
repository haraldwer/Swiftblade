#pragma once

#include "Blueprints/Blueprint.h"
#include "ECS/UniqueComponent.h"
#include "Property/Property.h"

namespace ECS
{
    class PlayerSpawner : public UniqueComponent<PlayerSpawner>
    {
    public:
        void Init() override;

        PROPERTY_C(ResBlueprint, BP, "Gameplay/Player/BP_Player.json")
    };
}
