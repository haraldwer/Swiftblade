#ifdef IMGUI_ENABLE

#include "Edit.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

String Utility::GetEditName(const String &InName, const uint32 InOffset)
{
    if (InOffset == 0)
        return InName + "##PropertyEdit";
    return "##" + InName + "_" + ToStr(InOffset);
}

bool Utility::BeginTable(const String &InName, const uint32 InOffset)
{
    if (InOffset == 0)
        return true;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    
    if (InName.find_first_of("##") == 0)
    {
        ImGui::SetNextItemWidth(-1);
        return true;
    }

    float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x;
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

void Utility::EndTable(const String& InName, const uint32 InOffset)
{
    if (InOffset == 0)
        return;
    ImGui::PopStyleVar();
    if (InName.find_first_of("##") == 0)
        return;
    ImGui::EndTable();
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

void Utility::Label(const String &InStr)
{
    ImGui::Text(InStr.c_str());
}

bool Utility::Checkbox(const String &InName, bool &InValue, const uint32 InOffset)
{
    return ImGui::Checkbox(GetEditName(InName, InOffset).c_str(), &InValue);
}

bool Utility::Int(const String &InName, int &InValue, const uint32 InOffset)
{
    return ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &InValue);
}

bool Utility::Int2(const String &InName, Vec2I &InValue, const uint32 InOffset)
{
    return ImGui::InputInt2(GetEditName(InName, InOffset).c_str(), InValue.data);
}

bool Utility::Int3(const String &InName, Vec3I &InValue, const uint32 InOffset)
{
    return ImGui::InputInt3(GetEditName(InName, InOffset).c_str(), InValue.data);
}

bool Utility::Int4(const String &InName, Vec4I &InValue, const uint32 InOffset)
{
    return ImGui::InputInt4(GetEditName(InName, InOffset).c_str(), InValue.data);
}

bool Utility::Float(const String &InName, float &InValue, const uint32 InOffset)
{
    return ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), &InValue);
}

bool Utility::Float2(const String &InName, Vec2F &InValue, const uint32 InOffset)
{
    return ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), InValue.data);
}

bool Utility::Float3(const String &InName, Vec3F &InValue, const uint32 InOffset)
{
    return ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), InValue.data);
}

bool Utility::Float4(const String &InName, Vec4F &InValue, const uint32 InOffset)
{
    return ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), InValue.data);
}

bool Utility::Text(const String &InName, String& InValue, const uint32 InOffset)
{
    return ImGui::InputText(GetEditName(InName, InOffset).c_str(), &InValue);
}

Vec2F Utility::GetCursorPos()
{
    auto p = ImGui::GetCursorPos();
    return { p.x, p.y };
}

void Utility::SetCursorPos(const Vec2F &InPos)
{
    ImGui::SetCursorPos({ InPos.x, InPos.y });
}

float Utility::GetWindowWidth()
{
    return ImGui::GetWindowWidth();
}

void Utility::SetNextItemWidth(const float InW)
{
    ImGui::SetNextItemWidth(InW);
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
