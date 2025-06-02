#include "Combat.h"

#include "CombatStateMachine.h"
#include "ImGui/imgui.h"
#include "Instance/Instance.h"

void ECS::Combat::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    stateMachine = new CombatStateMachine();
    stateMachine->Init();
}

void ECS::Combat::Update()
{
    if (stateMachine)
        stateMachine->Update();
}

bool ECS::Combat::EditState() const
{
    if (!stateMachine)
        return false;
    if (ImGui::Button("Save##Combat"))
        stateMachine->SaveConfig();
    return stateMachine->Edit();
}
