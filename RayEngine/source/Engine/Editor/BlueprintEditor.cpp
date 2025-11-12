#include "BlueprintEditor.h"

#ifdef IMGUI_ENABLE
#include "EditorFunctions.h"
#include "ImGui/imgui.h"
#endif

#include "raylib.h"
#include "Blueprints/Blueprint.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Environment.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Manager.h"
#include "Game/Instances/GameInstance.h"

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
    LOG("Editing BP: " + InBP.Identifier().Str())
    
    if (instanceID != ECS::INVALID_ID)
        ECS::Manager::Get().DestroyEntity(instanceID);
    
    config.Blueprint = InBP;
    config.SaveConfig();
    if (const BlueprintResource* bp = config.Blueprint.Get().Get())
        instanceID = bp->Instantiate();
    if (instanceID == ECS::INVALID_ID)
        instanceID = ecs.CreateEntity();
    
    selectedID = instanceID;

    if (const auto attr = ecs.GetComponent<ECS::Attributes>(instanceID))
    {
        if (attr->Name.Get().empty() || attr->Name.Get().starts_with("Unnamed"))
        {
            String name = Utility::File::Name(InBP.Identifier().Str());
            if (name.starts_with("BP_"))
                name = name.substr(3);
            if (name.ends_with(".json"))
                name = name.substr(0, name.size() - 5);
            attr->Name = name;
        }
    }
    
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

    if (Input::Action::Get("Back").Pressed())
    {
        LOG("Leaving editor");
        Engine::Manager::Get().Pop();
    }
}

void BlueprintEditor::Frame()
{
    ecs.Frame();
    
    if (ecs.GetSystem<ECS::SysEnvironment>().Empty())
    {
        Rendering::EnvironmentInstance i;
        i.skybox = config.DefaultSkybox;
        GetRenderScene().AddEnvironment(i);
    }

    GetRenderScene().AddDebugFunc([&]
    {
        DrawGrid(25, 1);
    });
    
    Instance::Frame();
}

#ifdef IMGUI_ENABLE

void BlueprintEditor::DrawDebugPanel()
{
    if (instanceID == ECS::INVALID_ID)
        return; 

    bool save = false;
    if (ImGui::Button("Save"))
        save = true;
    
    ImGui::SameLine();
    ImGui::Checkbox("Autosave", &config.Autosave.Get());
    
    if (ImGui::Button("Close", ImVec2(-1, 0)))
    {
        LOG("Leaving editor");
        Engine::Manager::Get().Pop();
    }

    ImGui::SeparatorText("Hierarchy");
    if (EditHierarchy(instanceID, selectedID))
        save |= config.Autosave.Get();
        
    ImGui::SeparatorText("Components");
    if (EditComponents(selectedID))
        save |= config.Autosave.Get();
    
    if (save)
        if (BlueprintResource* bp = config.Blueprint.Get().Get())
            bp->SaveEntity(instanceID);
}

#else
void BlueprintEditor::DrawDebugPanel() {  }
#endif