#pragma once

#include "Core/Debug/Window.h"

class ReferenceViewer : public Debug::Window
{
    String DebugWindowName() const override { return "Reference Viewer"; }
    void DrawDebugWindow() override;
};