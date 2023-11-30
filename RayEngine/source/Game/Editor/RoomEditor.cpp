#include "RoomEditor.h"

#include "Game/Game.h"
#include "Engine/Instance/Manager.h"
#include "ImGui/imgui.h"

void RoomEditor::Init()
{
    Instance::Init();
    CurrConfig.LoadConfig();
    
    OpenScene(); 
}

void RoomEditor::Deinit()
{
    CurrConfig.SaveConfig();
    VolumeEditor.Deinit();
    ObjectEditor.Deinit();
    ConnectionEditor.Deinit();
    Scene.Destroy();
    Camera.Deinit();
    Instance::Deinit();
}

void RoomEditor::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);

    if (IsKeyPressed(KEY_TAB))
        Camera.ToggleRequireHold();

    if (IsKeyPressed(KEY_ONE))
        Mode = EditMode::VOLUME;
    if (IsKeyPressed(KEY_TWO))
        Mode = EditMode::OBJECTS;
    if (IsKeyPressed(KEY_THREE))
        Mode = EditMode::CONNECTIONS;
    
    if (Camera.IsFullyControlling())
    {
        switch (Mode)
        {
        case EditMode::VOLUME:
            VolumeEditor.Update(InDelta);
            break;
        case EditMode::OBJECTS:
            ObjectEditor.Update(InDelta); 
            break;
        case EditMode::CONNECTIONS:
            ConnectionEditor.Update(InDelta); 
            break; 
        }
    }

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
    if (ImGui::Begin("Room Editor"))
    {
        if (CurrConfig.Edit())
            OpenScene();

        ImGui::Text(("Entities: " + std::to_string(Scene.Entities.size())).c_str());
        ImGui::Text(("ECS Entities: " + std::to_string(ECS.GetAllEntities().size())).c_str());

        switch (Mode)
        {
        case EditMode::VOLUME:
            VolumeEditor.UpdateUI();
            break;
        case EditMode::OBJECTS:
            ObjectEditor.UpdateUI(Camera.IsFullyControlling()); 
            break;
        case EditMode::CONNECTIONS:
            ConnectionEditor.UpdateUI(); 
            break;
        }
        
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
    Scene.Destroy();
    ECS.DestroyPending();
    VolumeEditor.Deinit();
    ObjectEditor.Deinit();
    ConnectionEditor.Deinit();
    VolumeEditor = {};
    ObjectEditor = {};
    ConnectionEditor  = {};

    if (const auto scene = CurrConfig.Scene.Get().Get())
    { 
        Scene = scene->Create();
        VolumeEditor.Init();
        ObjectEditor.Init();
        ConnectionEditor.Init(); 
    }
    
    Camera.SetRequireHold(false); 
}

void RoomEditor::PlayScene()
{
    if (Game* game = Engine::Manager::Get().Push<Game>())
        game->PlayScene(CurrConfig.Scene, Camera.GetPosition());
}

void RoomEditor::SaveRoom()
{
    auto& man = ECS::Manager::Get();
    Scene.Entities = man.GetAllEntities();
    Scene.Entities.erase(ObjectEditor.GetEditEntity());
    Scene.Entities.erase(ConnectionEditor.GetStartEntity());
    if (const auto scene = CurrConfig.Scene.Get().Get())
        scene->Save(Scene, ConnectionEditor.GetStartOffset());
}
