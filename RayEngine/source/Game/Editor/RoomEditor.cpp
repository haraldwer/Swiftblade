#include "RoomEditor.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Game/GameInstance.h"
#include "Engine/Instance/Manager.h"
#include "ImGui/imgui.h"

void RoomEditor::Init()
{
    Instance::Init();
    CurrConfig.LoadConfig();
    
    OpenScene();
    SubEditorManager.SetMode(SubEditorMode::VOLUME);
}

void RoomEditor::Deinit()
{
    CurrConfig.SaveConfig();
    SubEditorManager.Deinit();
    Camera.Deinit();
    Instance::Deinit();
}

void RoomEditor::Update(double InDelta)
{
    Time.Tick(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);

    if (IsKeyPressed(KEY_TAB))
        Camera.ToggleRequireHold();

    SubEditorMode newMode = SubEditorMode::COUNT;  
    if (IsKeyPressed(KEY_ONE))
        newMode = SubEditorMode::VOLUME;
    if (IsKeyPressed(KEY_TWO))
        newMode = SubEditorMode::OBJECTS;
    if (IsKeyPressed(KEY_THREE))
        newMode = SubEditorMode::CONNECTIONS;
    if (newMode != SubEditorMode::COUNT)
        SubEditorManager.SetMode(newMode); 
    
    SubEditorManager.Update(InDelta, Camera.IsFullyControlling()); 

    // Keyboard shortcuts
    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
            SaveRoom();
        if (IsKeyPressed(KEY_P))
            PlayScene();
    }
}

void RoomEditor::UpdateUI()
{
    ECS.UpdateUI();
    
    if (ImGui::Begin("Room Editor"))
    {
        if (Camera.IsControlling())
            ImGui::SetWindowFocus(nullptr); 
        
        if (CurrConfig.Edit())
            OpenScene();

        ImGui::Text(("Entities: " + std::to_string(Scene.Entities.size())).c_str());
        ImGui::Text(("ECS Entities: " + std::to_string(ECS.GetAllEntities().size())).c_str());

        SubEditorManager.UpdateUI(Camera.IsFullyControlling());
        
        if (ImGui::Button("Save"))
            SaveRoom(); 
        ImGui::SameLine();
        if (ImGui::Button("Play"))
            PlayScene();
    }
    ImGui::End(); 
}

void RoomEditor::OpenScene()
{
    SubEditorManager.Deinit();
    SubEditorManager = {};
    Scene.Destroy();
    ECS.DestroyPending();

    if (const auto scene = CurrConfig.Scene.Get().Get())
    { 
        Scene = scene->Create();
        SubEditorManager.Init(); 
    }
    
    Camera.SetRequireHold(false); 
}

void RoomEditor::PlayScene()
{
    if (GameInstance* game = Engine::Manager::Get().Push<GameInstance>())
        game->PlayScene(CurrConfig.Scene, Camera.GetPosition());
}

void RoomEditor::SaveRoom()
{
    auto& man = ECS::Manager::Get();
    for (auto e : man.GetAllEntities())
    {
        const auto trans = man.GetComponent<ECS::Transform>(e);
        CHECK_CONTINUE(!trans);
        CHECK_CONTINUE(trans->GetParent() != ECS::InvalidID);
        CHECK_CONTINUE(SubEditorManager.IgnoreSave(e));
        Scene.Entities.insert(e);
    }
    
    if (const auto scene = CurrConfig.Scene.Get().Get())
        scene->Save(Scene, SubEditorManager.GetStartOffset());
}
