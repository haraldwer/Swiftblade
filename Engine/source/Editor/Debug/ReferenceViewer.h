#pragma once

#include "Core/Debug/Panel.h"

namespace Editor
{
    class ReferenceViewer : public Debug::Panel
    {
        String PanelName() const override { return "References"; }
        void DrawPanel() override;
    };
}
