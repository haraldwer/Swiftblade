#include "Impl.h"

#include <filesystem>

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "Editor/FileBrowser/imgui_filebrowser.h"

String Resource::Base::Pick(const String& InLabel, const String& InID)
{
    auto find = InLabel.find_first_of("##");
    String substr = find == String::npos ? InLabel : InLabel.substr(0, find);
    if (!substr.empty())
    {
        ImGui::Text((substr + ": ").c_str());
        ImGui::SameLine(); 
    }
    if (ImGui::Button((InID + "##" + InLabel).c_str()))
        ImGui::OpenFileBrowser(InID);
    String result; 
    if (ImGui::FetchFileBrowserResult(InID, result))
        return result;
    return InID; 
}

bool Resource::Base::SaveButton(const String& InID)
{
    return ImGui::Button(("Save##" + InID).c_str());
}

