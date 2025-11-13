#include "AssetDetailPanel.h"

#include "Utility/String/StringConversionPath.h"
#include "Utility/String/StringConversionTime.h"

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
            size = Utility::File::GetSize(path);
            lastWriteTime = Utility::File::GetWriteTime(path);
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
                    content = Utility::File::Read(path);
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
                Utility::File::Write(path, content);
                saved = true;   
            }
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::MenuItem(("Reload##" + path).c_str()))
                content = Utility::File::Read(path);
            ImGui::EndMenuBar();
        }
    }
    else
    {
        if (ImGui::Button(("Save##" + path).c_str()))
        {
            Utility::File::Write(path, content);
            saved = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(("Reload##" + path).c_str()))
            content = Utility::File::Read(path);
    }
    
    ImGui::InputTextMultiline(("##" + path).c_str(), &content, ImVec2(-1, -1));

    if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        Utility::File::Write(path, content);
        saved = true;
    }
    
    return saved;
}
