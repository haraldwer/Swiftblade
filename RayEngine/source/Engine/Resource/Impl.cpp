#include "Impl.h"

#include <filesystem>

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/CustomFileBrowser/imgui_filebrowser.h"

bool Resource::Base::BeginEdit(const String& InID)
{
    return ImGui::CollapsingHeader(InID.c_str());
}

void Resource::Base::EndEdit()
{
    ImGui::EndSection(); 
}

String Resource::Base::Pick(const String& InLabel, const String& InID)
{
    ImGui::Text((InLabel + ": ").c_str());
    ImGui::SameLine(); 
    if (ImGui::Button((InID + "##" + InLabel).c_str()))
        ImGui::OpenFileBrowser(InID);
    String result; 
    if (ImGui::FetchFileBrowserResult(result))
        return result;
    return InID; 
}

bool Resource::Base::SaveButton(const String& InID)
{
    return ImGui::Button(("Save##" + InID).c_str());
}

