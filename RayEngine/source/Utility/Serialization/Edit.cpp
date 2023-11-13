#include "Edit.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"

bool Utility::Edit(const String& InName, bool& InOutData)
{
    return ImGui::Checkbox(InName.c_str(), &InOutData);
}

bool Utility::Edit(const String& InName, float& InOutData)
{
    return ImGui::InputFloat(InName.c_str(), &InOutData);
}

bool Utility::Edit(const String& InName, int32& InOutData)
{
    return ImGui::InputInt(InName.c_str(), &InOutData);
}

bool Utility::Edit(const String& InName, uint8& InOutData)
{
    int i = InOutData;
    const bool result = ImGui::InputInt(InName.c_str(), &i);
    InOutData = static_cast<uint8>(i); 
    return result; 
}

bool Utility::Edit(const String& InName, uint32& InOutData)
{
    int i = static_cast<int>(InOutData);
    const bool result = ImGui::InputInt(InName.c_str(), &i);
    InOutData = static_cast<uint32>(i); 
    return result; 
}

bool Utility::Edit(const String& InName, Vec3F& InOutData)
{
    return ImGui::InputFloat3(InName.c_str(), &InOutData.data[0]);
}

bool Utility::Edit(const String& InName, Vec4F& InOutData)
{
    return ImGui::InputFloat4(InName.c_str(), &InOutData.data[0]);
}

bool Utility::Edit(const String& InName, QuatF& InOutData)
{
    return ImGui::InputFloat4(InName.c_str(), &InOutData.data[0]);
}

bool Utility::Edit(const String& InName, Mat4F& InOutData)
{
    // TODO: Break down, edit individually
    return false; 
}

bool Utility::Edit(const String& InName, String& InOutData)
{
    return ImGui::InputText(InName.c_str(), &InOutData);
}

bool Utility::BeginList(const String& InName)
{
    return ImGui::BeginListBox(InName.c_str());
}

void Utility::EndList()
{
    ImGui::EndListBox();
}

void Utility::SameLine()
{
    ImGui::SameLine();
}

bool Utility::AddButton()
{
    return ImGui::Button("+");
}

bool Utility::RemoveButton()
{
    return ImGui::Button("-");
}
