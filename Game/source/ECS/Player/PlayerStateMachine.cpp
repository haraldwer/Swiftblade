#include "PlayerStateMachine.h"

#ifdef IMGUI_ENABLE
#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#endif

#include "Core/StateMachine/StateBase.h"

void PlayerStateMachine::Init()
{
    LoadConfig();
    StateMachine::Init();
}

#ifdef IMGUI_ENABLE

bool PlayerStateMachine::Edit(const String& InName, const uint32 InOffset)
{
    String current = {};
    if (const auto s = GetCurrentState())
        current += s->GetName();
    ImGui::Text("Current: %s", current.c_str());
    bool edited = false;
    for (StateBase* s : states)
    {
        String str = String("-- ") + s->GetName() + String(" --");
        if (ImGui::CollapsingHeader(str.c_str()))
        {
            if (ImGui::BeginSection(false))
            {
                if (s->EditState())
                    edited = true;
            }
            ImGui::EndSection();
        }
    }
	
    edited = edited || PropertyOwner::Edit(InName, InOffset);
    return edited;
}

#else
bool PlayerStateMachine::Edit(const String &InName, uint32 InOffset) {  }
#endif

void PlayerStateMachine::CustomSerialize(SerializeObj& InOutObj) const
{
    for (const StateBase* s : states)
    {
        InOutObj.Key(s->GetName().c_str());
        s->SerializeState(InOutObj);
    }
}

bool PlayerStateMachine::CustomDeserialize(const DeserializeObj& InObj)
{
    for (StateBase* s : states)
    {
        String name = s->GetName();
        if (InObj.HasMember(name.c_str()) && InObj[name.c_str()].IsObject())
            s->DeserializeState(InObj[name.c_str()].GetObj());
    }
    return true;
}
