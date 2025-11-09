#include "AssetDetailPanel.h"

void AssetDetailPanel::Draw()
{
    ImGui::Text("Asset: %s", path.c_str());
    ImGui::Text("No details available...");
}
