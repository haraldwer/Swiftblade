#pragma once

#include "Debug/Panel.h"
#include "ECS/Entity.h"

class Inspector : public Debug::Panel
{
public:
    void DrawPanel() override;
    String PanelName() const override { return "Inspector"; }

private:
    ECS::EntityID selected = ECS::INVALID_ID;
    ECS::EntityID hierarchySelected = ECS::INVALID_ID;
};
