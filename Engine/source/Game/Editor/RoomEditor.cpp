#include "RoomEditor.h"

#include "Database/Manager.h"
#include "Database/Data/RPCRoom.h"
#include "ECS/Volume/CubeVolume.h"
#include "Engine/Instance/Manager.h"
#include "Instances/GameInstance.h"
#include "ImGui/imgui.h"
#include "Menus/MenuRoomEditor.h"
#include "Rendering/Manager.h"
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
    {
        workingRoom = res->data;
        auto& type = workingRoom.Info.Get().Type.Get(); 
        if (type.empty())
            type = "ROOM";
    }
    
    subEditorManager.Init(this);
    if (!config.EditMode.Get().empty())
        subEditorManager.SetCurrent(config.EditMode.Get());
    
    freeCamera.Toggle(); 
    freeCamera.SetAlwaysEnabled(true);
    if (config.CamPos.Get() != Vec3F::Zero() && config.CamRot.Get() != Vec3F::Zero())
    {
        freeCamera.SetState(config.CamPos, config.CamRot);
    }
    else
    {
        freeCamera.SetTarget(Vec3F::One());
    }
    
    Input::Manager::Get().Push("RoomEditor");
}

void RoomEditor::Deinit()
{
    Input::Manager::Get().Pop("RoomEditor");

    config.CamPos = freeCamera.GetPosition();
    config.CamRot = freeCamera.GetRotation();
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
    freeCamera.Update();
    
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

        if (Input::Action::Get("Export").Pressed())
            ExportScene();
        
        // TODO: Reset camera?
    }

    if (Input::Action::Get("Back").Released())
        Engine::Manager::Get().Pop();
}

void RoomEditor::Frame()
{
    Rendering::EnvironmentInstance env;
    env.skybox = config.Skybox;
    GetRenderScene().AddEnvironment(env);
    
    subEditorManager.Frame();
    ecs.Frame(); 
    Instance::Frame();
}

void RoomEditor::SetRoom(const ResEditRoom &InRoom)
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
    ResScene tempScene = GetTempScene(ConvertRoomToScene());
    int repeats = workingRoom.Info.Get().Type.Get() == "ROOM" ? 3 : 1;
    if (auto game = Engine::Manager::Get().Push<GameInstance>())
        game->PlayScene({ tempScene, repeats, freeCamera.GetPosition() });
}

void RoomEditor::ExportScene()
{
    SaveRoom();
    SceneInstance scene = ConvertRoomToScene();
    //TODO: SetClipboardText(scene.ToStr(true).c_str());
    LOG("Scene copied to clipboard")
}

void RoomEditor::SaveRoom()
{
    CHECK_RETURN_LOG(!roomResource.Identifier().IsValid(), "Cannot save room, invalid room resource");
    auto res = roomResource.Get();
    CHECK_RETURN_LOG(!res, "Failed to load resource");
    UpdateRoomInfo();
    res->data = workingRoom;
    if (res->Save())
    {
        LOG("Room saved!")
        SaveThumbnail(roomResource.Identifier().Str() + ".png");
    }
    else
    {
        LOG("Failed to save room")
    }
}

void RoomEditor::SaveThumbnail(const String &InPath)
{
    /*
    auto& man = Rendering::Manager::Get();
    auto tex = man.mainViewport.GetFrameTexture();
    CHECK_ASSERT(!tex, "Invalid texture");
    Image img = LoadImageFromTexture(*tex);

    // Fixed width
    constexpr int width = 1024;
    int h = img.height * (static_cast<float>(width) / img.width);
    int w = width;
    
    ImageResize(&img, w, h);
    ImageFlipVertical(&img);
    
    if (ExportImage(img, InPath.c_str()))
    {
        LOG("Thumbnail created successfully");
    }
    else
    {
        LOG("Failed to export thumbnail");
    }
    */
    // TODO:
}

SceneInstance RoomEditor::ConvertRoomToScene()
{
    SceneInstance scene;
    auto& volEditor = GetSubEditors().Get<RoomVolumeEditor>();
    auto& objEditor = GetSubEditors().Get<RoomObjectEditor>();
    auto& conEditor = GetSubEditors().Get<RoomConnectionEditor>();

    // Reuses existing instantiated objects
    scene.entities.insert(conEditor.GetEnd(true));
    scene.entities.insert(conEditor.GetStart());
    scene.entities.insert(volEditor.GetCubeVolumeID());
    for (auto& obj : workingRoom.Objects.Get())
        scene.entities.insert(objEditor.LoadObject(obj.second));
    
    scene.offset = conEditor.GetWorldOffset();
    return scene;
}

ResScene RoomEditor::GetTempScene(const SceneInstance &InScene)
{
    Resource::ID id = Resource::ID("EditorRoom", true);
    ResScene resScene = ResScene(id);
    SceneResource* sceneResource = resScene.Get();
    CHECK_ASSERT(!sceneResource, "Invalid scene resource");
    sceneResource->FromInstance(InScene);
    return resScene;
}

void RoomEditor::UpdateRoomInfo()
{
    CHECK_RETURN_LOG(!roomResource.Identifier().IsValid(), "Cannot submit room, invalid room resource");
    
    RoomInfo room;
    room.Name = workingRoom.Info.Get().Name;
    room.Type = workingRoom.Info.Get().Type;
    if (room.Type.Get().empty())
        room.Type = "ROOM";
    
    auto& volEditor = GetSubEditors().Get<RoomVolumeEditor>();
    auto& vol = volEditor.GetVolume();
    room.Size = (vol.cachedMaxBounds - vol.cachedMinBounds).To<int>();
    room.Length = 0;
    Vec3F prev;
    for (auto& p : workingRoom.Path.Get())
    {
        Vec3F pos = vol.CoordToPos(p);
        if (prev == Vec3F::Zero())
        {
            prev = pos;
            continue;
        }
        room.Length += (pos - prev).Length();
        prev = pos;
    }
    
    for (auto& o : workingRoom.Objects.Get())
    {
        if (o.second.Object.Get() == "Spawner")
            room.Spawners++;
        else
            room.Objects++;
    }
    
    workingRoom.Info = room;
}

bool RoomEditor::SubmitRoom()
{
    SceneInstance scene = ConvertRoomToScene();
    String sceneStr = scene.ToStr(false);
    CHECK_RETURN_LOG(sceneStr.empty(), "Invalid submit request", false);
    
    DB::RPCSubmitRoom::Request req;
    req.Info = workingRoom.Info;
    req.Scene = sceneStr;
    LOG("Scene: ")
    LOG(req.Scene.Get());
    DB::Manager::Get().rpc.Request<DB::RPCSubmitRoom>(req);
    return true;
}

void RoomEditor::FinalizeSubmit() const
{
    // Delete the room file
    CHECK_RETURN_LOG(!roomResource.Identifier().IsValid(), "Invalid room resource");
    String path = roomResource.Identifier().Str();
    if (!Utility::File::Delete(path + ".png"))
        LOG("Failed to delete room thumbnail")
    if (!Utility::File::Delete(path))
        LOG("Failed to delete room file");
}

#ifdef IMGUI_ENABLE

void RoomEditor::DrawPanel()
{
    if (IsFreecam())
        ImGui::SetWindowFocus(nullptr); 
    ImGui::Text("Entities: %i", static_cast<int>(ecs.GetAllEntities().size()));
    ImGui::Text("History: %i", history.Count());

    if (ImGui::CollapsingHeader("Room"))
        workingRoom.Edit("Room");
    
    subEditorManager.DebugDraw();
}

#else
void RoomEditor::DrawDebugPanel() {  }
#endif

bool RoomEditor::CanEdit() const
{
    if (IsFreecam())
        return false;
    if (menu->GetUI().IsHovered())
        return false;
#ifdef IMGUI_ENABLE
    if (ImGui::IsAnyItemActive())
        return false;
#endif
    return true; 
}
