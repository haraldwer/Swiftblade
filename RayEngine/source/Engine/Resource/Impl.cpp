#include "Impl.h"

#include <filesystem>

#include "ImGui/imgui.h"
#include "ImGui/FileBrowser/ImGuiFileBrowser.h"
#include "Utility/String.h"

String Resource::Base::Pick(const String& InLabel, const String& InID)
{
    static bool pickFile = false;
    static bool pickFileInit = false;
    
    ImGui::Text((InLabel + ": ").c_str());
    ImGui::SameLine(); 
    if (ImGui::Button((InID + "##" + InLabel).c_str()))
    {
        pickFile = true;
        pickFileInit = false;
    }
    
    if (pickFile)
    {
        static imgui_addons::ImGuiFileBrowser fileBrowser;

        const String label = ("Open File##" + InLabel);
        
        if (!pickFileInit)
        {
            pickFileInit = true;
            fileBrowser = {};
            ImGui::OpenPopup(label.c_str());
        }
        
        if(fileBrowser.showFileDialog(label, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(0, 0), "*.*"))
        {
            pickFile = false;
            pickFileInit = false;
            
            const auto relativePath = std::filesystem::relative(fileBrowser.selected_path);
            const String file = Utility::StringRemove(relativePath.string(), "..\\content\\");
            return file;
        }
    }

    return InID; 
}
