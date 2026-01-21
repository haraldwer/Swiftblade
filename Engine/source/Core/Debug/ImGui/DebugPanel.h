#pragma once
#include "Debug/Panel.h"

namespace ImGui
{
    class DebugPanel : Debug::Panel
    {
    public:
        void DrawPanel() override;
        String PanelName() const override { return "ImGui Debugging"; }
    };
}
