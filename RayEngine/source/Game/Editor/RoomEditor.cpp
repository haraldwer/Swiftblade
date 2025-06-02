#include "RoomEditor.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Elements/RectImage.h"
#include "GameInstance.h"
#include "raylib.h"
#include "ImGui/imgui.h"

void RoomEditor::Init()
{
    Instance::Init();
    ecs.Init();
    currConfig.LoadConfig();
    
    OpenScene();
    subEditorManager.SetMode(SubEditorMode::PATH);
    
    EditorCamera.Toggle(); 
    EditorCamera.SetAlwaysEnabled(true);

    // Create UI
    UI::Builder builder = UI::Builder()
        .Push(UI::Container(UI::Transform::FromRect(Vec2F(600.0f, 80.0f), 20.0f)))
            .Add(UI::RectImage(ResTexture("UI/T_Rectangle.png"), UI::Margin(5.0f)))
            .Push(UI::List(UI::Transform::Fill(), 5.0f, 80.0f, UI::List::FlowDirection::HORIZONTAL))
                .Add(UI::Image(ResTexture("UI/T_Exit.png")), "Exit")
                .Add(UI::Image(ResTexture("UI/T_Save.png")), "Save")
                .Add(UI::Image(ResTexture("UI/T_Load.png")), "Load")
                .Add(UI::Image(ResTexture("UI/T_Play.png")), "Play")
                .Add(UI::Image(ResTexture("UI/T_ModeVolume.png")), "ModeVolume")
                .Add(UI::Image(ResTexture("UI/T_ModeObject.png")), "ModeObject")
                .Add(UI::Image(ResTexture("UI/T_ModeConnection.png")), "ModeConnection");
    ui = builder.Build();
}

void RoomEditor::Deinit()
{
    currConfig.SaveConfig();
    subEditorManager.Deinit();
    ecs.Deinit();
    Instance::Deinit();
}

void RoomEditor::Logic(const double InDelta)
{
    CHECK_ASSERT(!ui, "UI Invalid");
    
    Instance::Logic(InDelta);
    ecs.Update();
    EditorCamera.Update();
    ui->Update();
    
    SubEditorMode newMode = SubEditorMode::COUNT;  
    if (IsKeyPressed(KEY_ONE) || ui->Get("ModeVolume").IsClicked())
        newMode = SubEditorMode::VOLUME;
    if (IsKeyPressed(KEY_TWO) || ui->Get("ModeObject").IsClicked())
        newMode = SubEditorMode::OBJECTS;
    if (IsKeyPressed(KEY_THREE) || ui->Get("ModeConnection").IsClicked())
        newMode = SubEditorMode::PATH;
    if (newMode != SubEditorMode::COUNT)
        subEditorManager.SetMode(newMode); 
    
    subEditorManager.Update(EditorCamera.IsControlling());

    // Keyboard shortcuts
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Save").Pressed())
            SaveRoom();
        if (Input::Action::Get("Play").Pressed())
            PlayScene();
    }

    if (ui->Get("Save").IsClicked())
        SaveRoom();
    if (ui->Get("Play").IsClicked())
        PlayScene();
    if (ui->Get("Exit").IsClicked())
        Engine::Manager::Get().Pop();
}

void RoomEditor::Frame()
{
    ecs.Frame(); 
    Instance::Frame();
    
    CHECK_ASSERT(!ui, "UI Invalid");
    ui->Draw();
    subEditorManager.Frame(EditorCamera.IsControlling());
}

void RoomEditor::DrawDebugPanel()
{
    if (EditorCamera.IsControlling())
        ImGui::SetWindowFocus(nullptr); 
    
    if (currConfig.Edit())
        OpenScene();
    
    ImGui::Text("Entities: %i", static_cast<int>(scene.entities.size()));
    ImGui::Text("ECS Entities: %i", static_cast<int>(ecs.GetAllEntities().size()));

    subEditorManager.DebugDraw(EditorCamera.IsControlling()); 
    
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

    if (const auto scene = currConfig.Scene.Get().Get())
        scene = scene->Create();
    
    subEditorManager.Init(currConfig.IsArena ?
        RoomType::ARENA : RoomType::ROOM); 
}

void RoomEditor::PlayScene()
{
    if (GameInstance* game = Engine::Manager::Get().Push<GameInstance>())
        game->PlayScene(currConfig.Scene, EditorCamera.GetPosition());
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
    
    if (const auto scene = currConfig.Scene.Get().Get())
        scene->Save(scene, subEditorManager.GetStartOffset());
}
