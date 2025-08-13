#include "RoomEditor.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "raylib.h"
#include "ImGui/imgui.h"
#include "Menus/MenuRoomEditor.h"
#include "SubEditors/RoomObjectEditor.h"

void RoomEditor::Init()
{
    Instance::Init();
    ecs.Init();
    currConfig.LoadConfig();
    
    OpenScene();
    subEditorManager.SetCurrent(Type::Get<RoomObjectEditor>());
    
    editorCamera.Toggle(); 
    editorCamera.SetAlwaysEnabled(true);

    menu = Menu::Manager::Get().Push<MenuRoomEditor>();
}

void RoomEditor::Deinit()
{
    currConfig.SaveConfig();
    subEditorManager.Deinit();
    history.Clear();
    ecs.Deinit();
    Instance::Deinit();
}

void RoomEditor::Logic(const double InDelta)
{
    PROFILE();
    
    Instance::Logic(InDelta);
    ecs.Update();
    editorCamera.Update();
    
    SubEditorMode newMode = SubEditorMode::COUNT;  
    if (IsKeyPressed(KEY_ONE))
        newMode = SubEditorMode::VOLUME;
    if (IsKeyPressed(KEY_TWO))
        newMode = SubEditorMode::OBJECTS;
    if (IsKeyPressed(KEY_THREE))
        newMode = SubEditorMode::PATH;
    if (newMode != SubEditorMode::COUNT)
        subEditorManager.SetMode(newMode); 
    
    subEditorManager.Update(editorCamera.IsControlling());

    // Keyboard shortcuts
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Undo").Pressed())
            history.Undo();
        if (Input::Action::Get("Redo").Pressed())
            history.Redo();
        
        if (Input::Action::Get("Save").Pressed())
            SaveRoom();
        if (Input::Action::Get("Play").Pressed())
            PlayScene();
    }
}

void RoomEditor::Frame()
{
    EnvironmentInstance env;
    env.skybox = currConfig.Skybox;
    GetRenderScene().AddEnvironment(env);
    
    ecs.Frame(); 
    Instance::Frame();
    
    subEditorManager.Frame(editorCamera.IsControlling());
}

void RoomEditor::DrawDebugPanel()
{
    if (editorCamera.IsControlling())
        ImGui::SetWindowFocus(nullptr); 
    
    if (currConfig.Edit())
        OpenScene();
    
    ImGui::Text("Entities: %i", static_cast<int>(scene.entities.size()));
    ImGui::Text("ECS Entities: %i", static_cast<int>(ecs.GetAllEntities().size()));

    subEditorManager.DebugDraw(editorCamera.IsControlling()); 
    
    if (ImGui::Button("Save"))
        SaveRoom(); 
    ImGui::SameLine();
    if (ImGui::Button("Play"))
        PlayScene(); 
}

void RoomEditor::OpenScene()
{
    subEditorManager.Deinit();
    subEditorManager = {};
    scene.Destroy();
    ecs.DestroyPending();

    if (const auto sceneRes = currConfig.Scene.Get().Get())
        scene = sceneRes->Create();
    
    subEditorManager.Init();
}

void RoomEditor::PlayScene()
{
    if (GameInstance* game = Engine::Manager::Get().Push<GameInstance>())
        game->PlayScene(currConfig.Scene, editorCamera.GetPosition());
}

void RoomEditor::SaveRoom()
{
    auto& man = ECS::Manager::Get();
    for (auto e : man.GetAllEntities())
    {
        const auto trans = man.GetComponent<ECS::Transform>(e);
        CHECK_CONTINUE(!trans);
        CHECK_CONTINUE(trans->GetParent() != ECS::INVALID_ID);
        CHECK_CONTINUE(subEditorManager.IgnoreSave(e));
        scene.entities.insert(e);
    }
    
    if (const auto sceneRes = currConfig.Scene.Get().Get())
        sceneRes->Save(scene, subEditorManager.GetStartOffset());
}
