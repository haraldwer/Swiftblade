#pragma once

#include "Debug/Panel.h"
#include "ECS/Entity.h"

class Inspector : public Debug::Panel
{
public:
    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Inspector"; }

private:
    ECS::EntityID selected = ECS::INVALID_ID;
};
