#include "BlueprintEditor.h"

#include "EditorFunctions.h"
#include "Blueprints/Blueprint.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Manager.h"
#include "Game/Instances/GameInstance.h"
#include "ImGui/imgui.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    ecs.Init();
    editorCamera.Toggle(); 
    config.LoadConfig();
    if (pendingBP.Identifier().IsValid())
        config.Blueprint = pendingBP;
    SetBP(config.Blueprint);
}

void BlueprintEditor::Deinit()
{
    Instance::Deinit();
    ecs.Deinit();
    config.SaveConfig();
}

void BlueprintEditor::SetPendingBP(const ResBlueprint &InBP)
{
    pendingBP = InBP;
}

void BlueprintEditor::SetBP(const ResBlueprint& InBP)
{
    if (instanceID != ECS::INVALID_ID)
        ECS::Manager::Get().DestroyEntity(instanceID);
    
    config.Blueprint = InBP;
    config.SaveConfig();
    if (const BlueprintResource* bp = config.Blueprint.Get().Get())
        instanceID = bp->Instantiate();

    selectedID = instanceID;

    if (const auto t = ecs.GetComponent<ECS::Transform>(instanceID))
        editorCamera.SetTarget(t->GetPosition());
}

void BlueprintEditor::Logic(const double InDelta)
{
    Instance::Logic(InDelta);

    // Update
    ecs.Update();
    editorCamera.Update();

    if (instanceID == ECS::INVALID_ID)
        return; 
    
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Save").Pressed())
            if (BlueprintResource* bp = config.Blueprint.Get().Get())
                bp->SaveEntity(instanceID); 
        
        if (Input::Action::Get("Play").Pressed())
            Engine::Manager::Get().Push<GameInstance>();
    }
}

void BlueprintEditor::Frame()
{
    ecs.Frame(); 
    Instance::Frame();
}

void BlueprintEditor::DrawDebugPanel()
{
    if (config.Blueprint.Edit())
        SetBP(config.Blueprint);

    if (instanceID == ECS::INVALID_ID)
        return; 
    
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        if (BlueprintResource* bp = config.Blueprint.Get().Get())
            bp->SaveEntity(instanceID);

    ImGui::SeparatorText("Hierarchy");
    EditHierarchy(instanceID, selectedID);
    EditComponents(selectedID);
}
