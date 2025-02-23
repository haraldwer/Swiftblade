#include "Edit.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "Utility/Math/AngleConversion.h"

bool Utility::Edit(const String& InName, bool& InOutData, uint32 InOffset)
{
    return ImGui::Checkbox(GetEditName(InName, InOffset).c_str(), &InOutData);
}

bool Utility::Edit(const String& InName, float& InOutData, uint32 InOffset)
{
    return ImGui::InputFloat(GetEditName(InName, InOffset).c_str(), &InOutData);
}

bool Utility::Edit(const String& InName, int32& InOutData, uint32 InOffset)
{
    return ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &InOutData);
}

bool Utility::Edit(const String& InName, uint8& InOutData, uint32 InOffset)
{
    int i = InOutData;
    const bool result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
    InOutData = static_cast<uint8>(i); 
    return result; 
}

bool Utility::Edit(const String& InName, uint32& InOutData, uint32 InOffset)
{
    int i = static_cast<int>(InOutData);
    const bool result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
    InOutData = static_cast<uint32>(i); 
    return result; 
}

bool Utility::Edit(const String& InName, uint64& InOutData, uint32 InOffset)
{
    int i = static_cast<int>(InOutData);
    const bool result = ImGui::InputInt(GetEditName(InName, InOffset).c_str(), &i);
    InOutData = static_cast<uint32>(i); 
    return result; 
}

bool Utility::Edit(const String& InName, Vec2F& InOutData, uint32 InOffset)
{
    return ImGui::InputFloat2(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
}

bool Utility::Edit(const String& InName, Vec3F& InOutData, uint32 InOffset)
{
    return ImGui::InputFloat3(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
}

bool Utility::Edit(const String& InName, Vec4F& InOutData, uint32 InOffset)
{
    return ImGui::InputFloat4(GetEditName(InName, InOffset).c_str(), &InOutData.data[0]);
}

bool Utility::Edit(const String& InName, QuatF& InOutData, uint32 InOffset)
{
    Vec3F euler = InOutData.Euler();
    euler *= Math::RadiansToDegrees(1.0f);
    if (ImGui::InputFloat3(GetEditName(InName, InOffset).c_str(), &euler[0]))
    {
        InOutData = QuatF::FromEuler(euler *= Math::DegreesToRadians(1.0f));
        return true; 
    } 
    return false;  
}

bool Utility::Edit(const String& InName, Mat4F& InOutData, uint32 InOffset)
{
    // TODO: Break down, edit individually
    return false; 
}

bool Utility::Edit(const String& InName, String& InOutData, uint32 InOffset)
{
    String copy = InOutData;
    ImGui::InputText(GetEditName(InName, InOffset).c_str(), &InOutData);
    return InOutData != copy;
}

bool Utility::BeginList(const String& InName, uint32 InOffset)
{
    return ImGui::BeginListBox(GetEditName(InName, InOffset).c_str());
}

void Utility::EndList()
{
    ImGui::EndListBox();
}

void Utility::SameLine()
{
    ImGui::SameLine();
}

bool Utility::AddButton(uint32 InOffset)
{
    return ImGui::Button(GetEditName("+", InOffset).c_str());
}

bool Utility::RemoveButton(uint32 InOffset)
{
    return ImGui::Button(GetEditName("-", InOffset).c_str());
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
