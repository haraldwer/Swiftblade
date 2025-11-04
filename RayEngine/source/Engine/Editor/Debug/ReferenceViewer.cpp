#include "ReferenceViewer.h"

#ifdef IMGUI_ENABLE

#include "ImGui/imgui.h"
#include "Utility/Memory/Pointers.h"


void ReferenceViewer::DrawDebugPanel()
{
    ImGui::Text("Total references: %i", static_cast<int>(Utility::BaseRef::refs.size())); 

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    if (ImGui::BeginTable("References", 3, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Ref", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Strong", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Weak", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        
        for (const auto& res : Utility::BaseRef::refs)
        {
            CHECK_CONTINUE(!res.second);
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Ref");
            ImGui::TableNextColumn();
            CHECK_CONTINUE(!res.second); 
            ImGui::Text("%i", res.second->count);
            ImGui::TableNextColumn();
            ImGui::Text("%i", res.second->weakCount);
        }
        ImGui::PopStyleVar();
        ImGui::EndTable(); 
    }
}

#else

void ReferenceViewer::DrawDebugPanel() {  }

#endif