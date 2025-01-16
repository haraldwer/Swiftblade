#include "Combat.h"

#include "CombatStateMachine.h"
#include "ImGui/imgui.h"

void ECS::Combat::Init()
{
    StateMachine = new CombatStateMachine();
    StateMachine->LoadConfig();
    StateMachine->Init();
}

void ECS::Combat::Update()
{
    if (StateMachine)
        StateMachine->Update();
}

bool ECS::Combat::EditState() const
{
    if (!StateMachine)
        return false;
    if (ImGui::Button("Save##Combat"))
        StateMachine->SaveConfig();
    return StateMachine->Edit();
}
