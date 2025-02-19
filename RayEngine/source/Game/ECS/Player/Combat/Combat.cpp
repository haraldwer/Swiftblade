#include "Combat.h"

#include "CombatStateMachine.h"
#include "ImGui/imgui.h"
#include "Instance/Instance.h"

void ECS::Combat::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    StateMachine = new CombatStateMachine();
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
