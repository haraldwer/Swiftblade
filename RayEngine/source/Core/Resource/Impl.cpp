#include "Impl.h"

#ifdef IMGUI_ENABLE

#include "ImGui/FileBrowser/imgui_filebrowser.h"

#include "ImGui/imgui.h"

String Resource::Pick(const String& InLabel, const String& InID, const uint32 InOffset)
{
    String result = InID;
    bool valid = Utility::File::Exists(InID);
    
    if (Utility::BeginTable(InLabel, InOffset))
    {
        if (ImGui::BeginTable(("PickTable" + InLabel + Utility::ToStr(InOffset)).c_str(), 2, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
        {
            ImGui::TableSetupColumn("First", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Second", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFrameHeight());
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            
            if (!valid)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));

            String id = InID.empty() ? "Invalid" : InID;
            if (ImGui::Button((id + "##" + InLabel).c_str(), ImVec2(-1, 0)))
            {
                if (!valid)
                    ImGui::PopStyleColor();
                
                ImGui::OpenFileBrowser(InID);
            }
            else if (!valid)
                ImGui::PopStyleColor();
        
        
            ImGui::SetItemTooltip(InID.c_str());
        
            if (ImGui::BeginDragDropTarget())
            {
                dragDropCallback(result);
                ImGui::EndDragDropTarget();
            }
            
            ImGui::TableNextColumn();

            if (!InID.empty())
                if (ImGui::ArrowButton((InID + Utility::ToStr(InOffset)).c_str(), ImGuiDir_Right))
                    pickCallback(InID);
            
            ImGui::EndTable();
        }
        
        Utility::EndTable(InLabel, InOffset);
    }
    
    String pickResult; 
    if (ImGui::FetchFileBrowserResult(InID, pickResult))
        result = pickResult;
    
    return result;
}

bool Resource::SaveButton(const String& InID)
{
    return ImGui::Button(("Save##" + InID).c_str());
}

void Resource::ShowInvalid(const String& InID)
{
    
}

#else

String Resource::Pick(const String& InLabel, const String& InID) { return ""; }
bool Resource::SaveButton(const String& InID) { return false; }
void Resource::ShowInvalid(const String& InID) { }

#endif