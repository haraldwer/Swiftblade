#include "MenuLevelPlay.h"

#include "Database/Manager.h"
#include "Database/Data/RPCRoom.h"
#include "Instance/InstanceManager.h"
#include "Instances/GameInstance.h"
#include "UI/Builder.h"
#include "UI/Widgets/Common/LoadingSpinner.h"

void MenuLevelPlay::Init()
{
    ui = UI::Builder()
        .Add(UI::LoadingSpinner(UI::Transform::Centered()))
        .Build();

    ui.SetBackground({{0, 0, 0, 0.5}});

    onLevelInfo.Bind([&](const auto& InResp, auto InC)
    {
        ReceiveLevel(InResp);
    });

    onRoomInfo.Bind([&](const auto& InResp, auto InC)
    {
        ReceiveRoom(InResp);
    });

    loading = false;
}

void MenuLevelPlay::Update()
{
    Instance::Update();
    if (!loading)
        BeginLoading();
    ui.Update();
}

void MenuLevelPlay::SetLevel(const UI::LevelEntryData &InData, int InSeed)
{
    levelEntry = InData;
    config = LevelConfig();
    config.Seed = InSeed;
}

void MenuLevelPlay::BeginLoading()
{
    loading = true;
    ResLevel res = levelEntry.resource;
    if (!res.Identifier().IsValid())
    {
        if (levelEntry.entry.ID.Get().empty())
        {
            // TODO: Show error!
            LOG("Invalid id")
            return;
        }
        res = Utility::File::GetCachePath(levelEntry.entry.ID, ".json");
    }
        
    if (auto level = res.Get())
    {
        SetLevel(level->data);
    }
    else
    {
        DB::RPCLevelInfo::Request req;
        req.ID = levelEntry.entry.ID;
        DB::Manager::Get().rpc.Request<DB::RPCLevelInfo>(req);
    }
}

void MenuLevelPlay::ReceiveLevel(const DB::Response<DB::RPCLevelInfo>& InResponse)
{
    if (!InResponse.success)
    {
        // TODO: Show error
        LOG("Invalid response")
        return;
    }

    // Create cache level
    String path = Utility::File::GetCachePath(InResponse.data.ID, ".json");
    if (!InResponse.data.Data.Get().Save(path))
    {
        LOG("Failed to save: " + path)
    }
    
    SetLevel(InResponse.data.Data);
}

void MenuLevelPlay::SetLevel(const Level &InData)
{
    config.NumArenas = InData.NumArenas;
    config.NumRooms = InData.NumRooms;
    rooms.clear();
    arenas.clear();

    for (auto& room : InData.Rooms.Get())
    {
        rooms.push_back(room.ID);
        RequestRoom(room.ID);
    }
    for (auto& arena : InData.Arenas.Get())
    {
        arenas.push_back(arena.ID);
        RequestRoom(arena.ID);
    }
    
    TryFinishRoomLoading();
}

void MenuLevelPlay::RequestRoom(const String& InRoom)
{
    if (InRoom.empty())
    {
        // TODO: Show error!
        LOG("Invalid room id")
        return;
    }

    // Room has already been requested
    if (pendingRoomRequests.contains(InRoom))
        return;

    ResScene res = Utility::File::GetCachePath("scene_" + InRoom, ".json");
    if (res.Get())
    {
        sceneResources[InRoom] = res;
        return;
    }
    
    pendingRoomRequests.insert(InRoom);
    DB::RPCRoomInfo::Request req;
    req.ID = InRoom;
    DB::Manager::Get().rpc.Request<DB::RPCRoomInfo>(req);
}

void MenuLevelPlay::ReceiveRoom(const DB::Response<DB::RPCRoomInfo> &InResponse)
{
    String id = InResponse.data.Info.Get().ID.Get();
    CHECK_RETURN(!pendingRoomRequests.contains(id))
    
    if (!InResponse.success)
    {
        // TODO: Show error
        LOG("Invalid response")
        return;
    }

    // Create cache room
    String roomPath = Utility::File::GetCachePath(id, ".json");
    if (!InResponse.data.Info.Get().Save(roomPath))
    {
        // TODO: Show error
        LOG("Failed to save: " + roomPath)
        return; 
    }

    // Create cache scene
    String scenePath = Utility::File::GetCachePath("scene_" + id, ".json");
    if (!Utility::File::Write(scenePath, InResponse.data.Scene.Get()))
    {
        // TODO: Show error
        LOG("Failed to save: " + scenePath)
        return; 
    }

    // Verify resource
    ResScene scene = scenePath;
    if (!scene.Get())
    {
        LOG("Failed to read: " + scenePath);
        return;
    }
    
    sceneResources[id] = scene;
    pendingRoomRequests.erase(id);
    TryFinishRoomLoading();
}

void MenuLevelPlay::TryFinishRoomLoading()
{
    CHECK_RETURN(!pendingRoomRequests.empty())
    
    for (auto& room : rooms)
        config.Rooms.Get().push_back(sceneResources.at(room));
    for (auto& arena : arenas)
        config.Arenas.Get().push_back(sceneResources.at(arena));

    if (auto game = Engine::InstanceManager::Get().Push<GameInstance>())
        game->PlayLevel(config);
}
