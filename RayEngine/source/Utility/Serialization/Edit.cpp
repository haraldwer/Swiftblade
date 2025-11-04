#ifdef IMGUI_ENABLE

#include "Edit.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"

String Utility::GetEditName(const String &InName, const uint32 InOffset)
{
    if (InOffset == 0)
        return InName + "##PropertyEdit";
    return "##" + InName + "_" + ToStr(InOffset);
}

bool Utility::BeginTable(const String &InName, uint32 InOffset)
{
    if (InOffset == 0)
        return true;
    
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    if (ImGui::BeginTable((InName + ToStr(InOffset)).c_str(), 2, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
        
        ImGui::Text(InName.c_str());
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        return true;
    }
    return false;
}

void Utility::EndTable(uint32 InOffset)
{
    if (InOffset != 0)
    {
        ImGui::PopStyleVar();
        ImGui::EndTable();
    }
}

bool Utility::BeginSection(const String& InName)
{
    ImGui::Text("%s", InName.c_str());
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

void Utility::Indent()
{
    ImGui::Indent();
}

void Utility::Unindent()
{
    ImGui::Unindent();
}

void Utility::EditText(const String &InStr)
{
    ImGui::Text(InStr.c_str());
}

bool Utility::MaybeCollapse(const String& InName, const uint32 InOffset, bool& OutHeader)
{
    if (InName.empty() || InOffset == 0)
        return true;
    OutHeader = ImGui::CollapsingHeader((InName + GetEditName(InName, InOffset)).c_str());
    return OutHeader;
}

bool Utility::Button(const String& InName, const uint32 InOffset)
{
    return ImGui::Button(GetEditName(InName, InOffset).c_str());
}

#endif
