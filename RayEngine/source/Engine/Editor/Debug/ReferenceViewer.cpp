#include "ReferenceViewer.h"

#include <string>

#include "ImGui/imgui.h"
#include "Utility/Memory/Pointers.h"

void ReferenceViewer::DrawDebugPanel()
{
    ImGui::Text("Total references: %i", static_cast<int>(Utility::BaseRef::Refs.size())); 

    if (ImGui::BeginTable("References", 3, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Ref", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Strong", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Weak", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        
        for (const auto& res : Utility::BaseRef::Refs)
        {
            CHECK_CONTINUE(!res.second);
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Ref");
            ImGui::TableNextColumn();
            CHECK_CONTINUE(!res.second); 
            ImGui::Text("%i", res.second->Count);
            ImGui::TableNextColumn();
            ImGui::Text("%i", res.second->WeakCount);
        }
        ImGui::EndTable(); 
    }
}
