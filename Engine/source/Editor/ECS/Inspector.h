#pragma once

#include "Core/Debug/Panel.h"
#include "ECS/Entity.h"

namespace Editor
{
    class Inspector : public Debug::Panel
    {
    public:
        void DrawPanel() override;
        String PanelName() const override { return "Inspector"; }

    private:
        ECS::EntityID selected = ECS::INVALID_ID;
        ECS::EntityID hierarchySelected = ECS::INVALID_ID;
    };
}
