#pragma once

#include "Core/Debug/Panel.h"

class ReferenceViewer : public Debug::Panel
{
    String DebugPanelName() const override { return "References"; }
    void DrawDebugPanel() override;
};