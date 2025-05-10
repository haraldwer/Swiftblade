#include "Edit.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/imgui_stdlib.h"

String Utility::GetEditName(const String &InName, uint32 InOffset)
{
    return InName + "##PropertyEdit_" + std::to_string(InOffset);
}

bool Utility::BeginSection(const String& InName)
{
    ImGui::Text(InName.c_str());
    return ImGui::BeginSection(true);
}

void Utility::EndSection()
{
    ImGui::EndSection();
}

void Utility::SameLine()
{
    ImGui::SameLine();
}

void Utility::Separator()
{
    ImGui::Separator();
}

bool Utility::MaybeCollapse(const String& InName, uint32 InOffset, bool& OutHeader)
{
    if (InName.empty() || InOffset == 0)
        return true;
    OutHeader = ImGui::CollapsingHeader(GetEditName(InName, InOffset).c_str());
    return OutHeader;
}

bool Utility::Button(const String& InName, uint32 InOffset)
{
    return ImGui::Button(GetEditName(InName, InOffset).c_str());
}
