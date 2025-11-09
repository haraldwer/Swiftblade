#include "TextDetails.h"

#include "File/File.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"

void TextDetails::Init()
{
    content = Utility::ReadFile(path);
}

void TextDetails::Draw()
{
    ImGui::Text("Asset: %s", path.c_str());
    if (ImGui::Button("Save"))
        Utility::WriteFile(path, content);
    ImGui::SameLine();
    if (ImGui::Button("Reload"))
        content = Utility::ReadFile(path);
    ImGui::InputTextMultiline(("##" + path).c_str(), &content, ImVec2(-1, -1));
}

bool TextDetails::Accept(const String &InPath)
{
    if (InPath.ends_with(".txt") ||
        InPath.ends_with(".json") ||
        InPath.ends_with(".ini"))
        return true;
    if (Utility::Filename(InPath).starts_with("SH_") &&
            (InPath.ends_with(".fs") ||
            InPath.ends_with(".ds") ||
            InPath.ends_with(".vs") ||
            InPath.ends_with(".si")))
        return true;
    return false;
}
