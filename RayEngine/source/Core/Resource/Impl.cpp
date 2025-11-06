#include "Impl.h"

#ifdef IMGUI_ENABLE

#include "ImGui/FileBrowser/imgui_filebrowser.h"

#include "ImGui/imgui.h"

String Resource::Pick(const String& InLabel, const String& InID, uint32 InOffset)
{
    if (Utility::BeginTable(InLabel, InOffset))
    {
        if (ImGui::Button((InID + "##" + InLabel).c_str(), ImVec2(-1, 0)))
            ImGui::OpenFileBrowser(InID);
        else
            ImGui::SetItemTooltip(InID.c_str());
            
        Utility::EndTable(InOffset);
    }
    
    String result; 
    if (ImGui::FetchFileBrowserResult(InID, result))
        return result;
    return InID;
}

bool Resource::SaveButton(const String& InID)
{
    return ImGui::Button(("Save##" + InID).c_str());
}

#else

String Resource::Pick(const String& InLabel, const String& InID) { return ""; }
bool Resource::SaveButton(const String& InID) { return false; }

#endif