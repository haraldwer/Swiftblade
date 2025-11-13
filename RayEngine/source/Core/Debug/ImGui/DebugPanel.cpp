#include "DebugPanel.h"

#include "ImGui/imgui.h"

void ImGui::DebugPanel::DrawPanel()
{
    auto id = DockSpace(GetItemID(), ImVec2(-1, -1));
    
    SetNextWindowDockID(id);
    ShowDemoWindow();
    SetNextWindowDockID(id);
    ShowMetricsWindow();
    SetNextWindowDockID(id);
    ShowDebugLogWindow();
    SetNextWindowDockID(id);
    ShowStackToolWindow();
}
