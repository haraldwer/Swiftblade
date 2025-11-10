#include "AssetDetailPanel.h"

#include "Utility/String/StringConversionTime.h"
#include <filesystem>

#include "ImGui/imgui_custom.h"

void AssetDetailPanel::Draw()
{
    DrawFileInfo();
    ImGui::Text("No details available...");
}

void AssetDetailPanel::DrawFileInfo()
{
    if (ImGui::CollapsingHeader(path.c_str()))
    {
        // More info here!
        if (!infoSet)
        {
            std::filesystem::path p(path);
            size = std::filesystem::file_size(p);
            lastWriteTime = std::chrono::clock_cast<std::chrono::system_clock>(std::filesystem::last_write_time(path));
            infoSet = true;
        }

        ImGui::Text(("Size: " + Utility::ToStr(size)).c_str());
        ImGui::Text("Last write: %s", Utility::ToStr(lastWriteTime).c_str());
        
        if (AllowRawEdit())
        {
            if (rawEditing)
            {
                ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(10000, 10000));
                if (ImGui::Begin(("Text edit##" + path).c_str(), &rawEditing, ImGuiWindowFlags_MenuBar))
                {
                    RawTextEdit(true);
                    ImGui::End();
                }
            }
            
            if (ImGui::Button(("Text edit##" + path).c_str(), ImVec2(-1, 0)))
            {
                rawEditing = !rawEditing;
                if (rawEditing)
                    content = Utility::ReadFile(path);
            }
        }
    }
}

bool AssetDetailPanel::RawTextEdit(bool InSubwindow)
{
    bool saved = false;
    
    // Raw edit the file string content
    if (InSubwindow)
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem(("Save##" + path).c_str()))
            {
                Utility::WriteFile(path, content);
                saved = true;   
            }
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::MenuItem(("Reload##" + path).c_str()))
                content = Utility::ReadFile(path);
            ImGui::EndMenuBar();
        }
    }
    else
    {
        if (ImGui::Button(("Save##" + path).c_str()))
        {
            Utility::WriteFile(path, content);
            saved = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(("Reload##" + path).c_str()))
            content = Utility::ReadFile(path);
    }
    
    ImGui::InputTextMultiline(("##" + path).c_str(), &content, ImVec2(-1, -1));

    if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        Utility::WriteFile(path, content);
        saved = true;
    }
    
    return saved;
}
