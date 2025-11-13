#pragma once

#include "Core/Debug/Panel.h"

class ReferenceViewer : public Debug::Panel
{
    String PanelName() const override { return "References"; }
    void DrawPanel() override;
};