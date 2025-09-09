#include "RoomEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "ImGui/imgui.h"
#include "Menus/MenuRoomEditor.h"
#include "SubEditors/RoomConnectionEditor.h"
#include "SubEditors/RoomObjectEditor.h"
#include "SubEditors/RoomVolumeEditor.h"

void RoomEditor::Init()
{
    Instance::Init();

    config.LoadConfig();
    menu = menus.Push<MenuRoomEditor>();
    ecs.Init();

    if (!roomResource.Identifier().IsValid())
    {
        LOG("Invalid room resource, loading last room");
        roomResource = config.LastRoom;
    }
    
    if (auto res = roomResource.Get())
        workingRoom = res->Get();
    
    subEditorManager.Init(this);
    if (!config.EditMode.Get().empty())
        subEditorManager.SetCurrent(config.EditMode.Get());
    
    editorCamera.Toggle(); 
    editorCamera.SetAlwaysEnabled(true);
    if (config.CamPos.Get() != Vec3F::Zero() && config.CamRot.Get() != Vec3F::Zero())
    {
        editorCamera.SetState(config.CamPos, config.CamRot);
    }
    else
    {
        auto& volume = subEditorManager.Get<RoomConnectionEditor>().GetVolume();
        ECS::VolumeCoord center = volume.GetCenter();
        editorCamera.SetTarget(volume.CoordToPos(center));
    }
    
    Input::Manager::Get().Push("RoomEditor");
}

void RoomEditor::Deinit()
{
    Input::Manager::Get().Pop("RoomEditor");

    config.CamPos = editorCamera.GetPosition();
    config.CamRot = editorCamera.GetRotation();
    config.LastRoom = roomResource;
    config.EditMode = subEditorManager.GetCurrentName();
    config.SaveConfig();
    
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
    
    subEditorManager.Update();

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
            PlayRoom();

        // TODO: Reset camera?
    }

    if (Input::Action::Get("Back").Released())
        Engine::Manager::Get().Pop();
}

void RoomEditor::Frame()
{
    EnvironmentInstance env;
    env.skybox = config.Skybox;
    GetRenderScene().AddEnvironment(env);
    
    subEditorManager.Frame();
    ecs.Frame(); 
    Instance::Frame();
}

void RoomEditor::SetRoom(const ResRoom &InRoom)
{
    CHECK_RETURN_LOG(!InRoom.Identifier().IsValid(), "Invalid room: " + InRoom.Identifier().Str())
    roomResource = InRoom;
}

void RoomEditor::LoadRoom()
{
    Deinit();
    Init();
}

void RoomEditor::PlayRoom()
{
    SaveRoom();
    ResScene tempScene = ConvertRoomToScene();
    if (auto game = Engine::Manager::Get().Push<GameInstance>())
        game->PlayScene(tempScene, editorCamera.GetPosition());
}

void RoomEditor::SaveRoom()
{
    CHECK_RETURN_LOG(!roomResource.Identifier().IsValid(), "Invalid room resource");
    auto res = roomResource.Get();
    CHECK_RETURN_LOG(!res, "Failed to load resource");
    auto& room = res->Get();
    room = workingRoom;
    if (res->Save(roomResource.Identifier().Str()))
        LOG("Room saved!")
    else
        LOG("Failed to save room")
}

ResScene RoomEditor::ConvertRoomToScene()
{
    SceneInstance scene;

    auto& volEditor = GetSubEditors().Get<RoomVolumeEditor>();
    auto& objEditor = GetSubEditors().Get<RoomObjectEditor>();
    auto& conEditor = GetSubEditors().Get<RoomConnectionEditor>();

    // Reuses existing instantiated objects
    scene.entities.insert(conEditor.GetConnection(true));
    scene.entities.insert(volEditor.GetCubeVolumeID());
    for (auto& obj : workingRoom.Objects.Get())
        scene.entities.insert(objEditor.LoadObject(obj.second));

    Resource::ID id = Resource::ID("EditorRoom", true);
    ResScene resScene = ResScene(id);
    SceneResource* sceneResource = resScene.Get();
    CHECK_ASSERT(!sceneResource, "Invalid scene resource");
    sceneResource->FromInstance(scene);
    return resScene;
}

void RoomEditor::DrawDebugPanel()
{
    if (IsFreecam())
        ImGui::SetWindowFocus(nullptr); 
    ImGui::Text("Entities: %i", static_cast<int>(ecs.GetAllEntities().size()));
    ImGui::Text("History: %i", history.Count());
    subEditorManager.DebugDraw();
}

bool RoomEditor::CanEdit() const
{
    if (IsFreecam())
        return false;
    if (menu->GetUI().IsHovered())
        return false;
    if (ImGui::IsAnyItemActive())
        return false;
    return true; 
}
