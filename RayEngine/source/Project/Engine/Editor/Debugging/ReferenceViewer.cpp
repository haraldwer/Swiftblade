#include "ReferenceViewer.h"

#include <string>

#include "ImGui/imgui.h"
#include "Utility/Memory/Pointers.h"

void ReferenceViewer::DrawDebugWindow()
{
    ImGui::Text(("Total references: " + std::to_string(Utility::BaseRef::Refs.size())).c_str()); 

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
            ImGui::Text(std::to_string(res.second->Count).c_str());
            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(res.second->WeakCount).c_str());
        }
        ImGui::EndTable(); 
    }
}
