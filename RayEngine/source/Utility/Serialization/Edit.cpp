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

    float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    if (ImGui::BeginTable((InName + ToStr(InOffset)).c_str(), 2, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_SizingStretchSame, ImVec2(width, 0), width))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
        
        ImGui::Text(InName.substr(0, InName.find_first_of("##")).c_str());
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
    ImGui::Text("%s", InName.substr(0, InName.find_first_of("##")).c_str());
    return ImGui::BeginChild(InName.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY);
}

void Utility::EndSection()
{
    ImGui::EndChild();
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

bool CustomCollapse(const char* label)
{
    using namespace ImGui;
    PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    bool* p_open = GetStateStorage()->GetBoolRef(GetID(label), false);
    if (Button(label, ImVec2(-FLT_MIN, 0.0f)))
        *p_open ^= 1;
    ImGuiStyle& style = GetStyle();
    ImVec2 arrow_pos = ImVec2(GetItemRectMax().x - style.FramePadding.x - GetFontSize(), GetItemRectMin().y + style.FramePadding.y);
    RenderArrow(GetWindowDrawList(), arrow_pos, GetColorU32(ImGuiCol_Text), *p_open ? ImGuiDir_Down : ImGuiDir_Right);
    PopStyleVar();
    return *p_open;
}

bool Utility::MaybeCollapse(const String& InName, const uint32 InOffset, bool& OutHeader)
{
    if (InName.empty() || InOffset == 0)
        return true;
    OutHeader = ImGui::CollapsingHeader((InName + "##PropertyHeader" + ToStr(InOffset)).c_str());
    return OutHeader;
}

bool Utility::Button(const String& InName, const uint32 InOffset)
{
    return ImGui::Button((InName + "##" + ToStr(InOffset)).c_str());
}

#endif
