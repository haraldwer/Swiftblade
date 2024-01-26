#include "RoomEditor.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Game/GameInstance.h"
#include "Engine/Instance/Manager.h"
#include "Engine/UI/Builder.h"
#include "Engine/UI/Elements/List.h"
#include "Engine/UI/Elements/RectImage.h"
#include "ImGui/imgui.h"

void RoomEditor::Init()
{
    Instance::Init();
    ECS.Init();
    CurrConfig.LoadConfig();
    
    OpenScene();
    SubEditorManager.SetMode(SubEditorMode::VOLUME);

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
    
    UI = builder.Build(); 
}

void RoomEditor::Deinit()
{
    CurrConfig.SaveConfig();
    SubEditorManager.Deinit();
    Camera.Deinit();
    ECS.Deinit();
    Instance::Deinit();
}

void RoomEditor::Logic(double InDelta)
{
    Instance::Logic(InDelta);
    
    CHECK_ASSERT(!UI, "UI Invalid"); 
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);
    UI->Update();
    
    if (IsKeyPressed(KEY_TAB))
        Camera.ToggleRequireHold();
    
    SubEditorMode newMode = SubEditorMode::COUNT;  
    if (IsKeyPressed(KEY_ONE) || UI->Get("ModeVolume").IsClicked())
        newMode = SubEditorMode::VOLUME;
    if (IsKeyPressed(KEY_TWO) || UI->Get("ModeObject").IsClicked())
        newMode = SubEditorMode::OBJECTS;
    if (IsKeyPressed(KEY_THREE) || UI->Get("ModeConnection").IsClicked())
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

    if (UI->Get("Save").IsClicked())
        SaveRoom();
    if (UI->Get("Play").IsClicked())
        PlayScene();
    if (UI->Get("Exit").IsClicked())
        Engine::Manager::Get().Pop(); 
}

void RoomEditor::Frame(double InDelta)
{
    CHECK_ASSERT(!UI, "UI Invalid");
    
    UI->Draw();
    
    SubEditorManager.UpdateUI(Camera.IsFullyControlling());
}

void RoomEditor::DrawDebugWindow()
{
    if (Camera.IsControlling())
        ImGui::SetWindowFocus(nullptr); 
    
    if (CurrConfig.Edit())
        OpenScene();
    
    ImGui::Text(("Entities: " + std::to_string(Scene.Entities.size())).c_str());
    ImGui::Text(("ECS Entities: " + std::to_string(ECS.GetAllEntities().size())).c_str());

    SubEditorManager.DebugDraw(Camera.IsFullyControlling()); 
    
    if (ImGui::Button("Save"))
        SaveRoom(); 
    ImGui::SameLine();
    if (ImGui::Button("Play"))
        PlayScene(); 
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
        
        SubEditorManager.Init(CurrConfig.IsArena ?
            RoomType::ARENA : RoomType::ROOM); 
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
