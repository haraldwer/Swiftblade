#include "RoomEditor.h"

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
    
    room = config.WorkingRoom;
    subEditorManager.Init(this);
    
    editorCamera.Toggle(); 
    editorCamera.SetAlwaysEnabled(true);

    Input::Manager::Get().Push("RoomEditor");
}

void RoomEditor::Deinit()
{
    Input::Manager::Get().Pop("RoomEditor");
    
    config.WorkingRoom = room;
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
    }
}

void RoomEditor::Frame()
{
    EnvironmentInstance env;
    env.skybox = config.Skybox;
    GetRenderScene().AddEnvironment(env);
    
    ecs.Frame(); 
    Instance::Frame();
    
    subEditorManager.Frame();
}

void RoomEditor::OpenRoom(const Room& InRoom)
{
    Deinit();
    room = InRoom;
    Init();
}

void RoomEditor::PlayRoom()
{
    SaveRoom();
    ResScene tempScene = ConvertRoomToScene();
    if (GameInstance* game = Engine::Manager::Get().Push<GameInstance>())
        game->PlayScene(tempScene, editorCamera.GetPosition());
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
    for (auto& obj : room.Objects.Get())
        scene.entities.insert(objEditor.GetObject(obj, true));

    Resource::ID id = Resource::ID("EditorRoom", true);
    ResScene resScene = ResScene(id);
    SceneResource* sceneResource = resScene.Get();
    CHECK_ASSERT(!sceneResource, "Invalid scene resource");
    sceneResource->FromInstance(scene);
    return resScene;
}

void RoomEditor::SaveRoom()
{
    // What's the path to the current room?
    // Is the room actually a resource?? I don't know...
    //room.Save(""); 
}

void RoomEditor::DrawDebugPanel()
{
    if (IsFreecam())
        ImGui::SetWindowFocus(nullptr); 
    ImGui::Text("Entities: %i", static_cast<int>(ecs.GetAllEntities().size()));
    ImGui::Text("History: %i", history.Count());
    subEditorManager.DebugDraw();
}
