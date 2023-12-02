#include "Game.h"

#include <filesystem>

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"
#include "Systems/Player/Input.h"

void Game::Init()
{
    Instance::Init();
    Physics.Init();

    if (!StartScene.Identifier().empty())
    {
        Vector<ResScene> rooms;
        rooms.push_back(StartScene);
        rooms.push_back(StartScene);
        rooms.push_back(StartScene);
        StartScene.Unload(); // Force reload
        RoomManager.Load(rooms, false);
    }
    else
    {
        Vector<ResScene> rooms;
        
        // Start
        rooms.emplace_back("../content/Scenes/S_Start.json");
        
        // Load content of room folder
        const std::filesystem::path path = "../content/Scenes/Rooms";
        for (const auto& entry : std::filesystem::directory_iterator(path))
            if (entry.path().extension() == ".json")
                rooms.emplace_back(entry.path().string());

        // End
        rooms.emplace_back("../content/Scenes/S_End.json");

        RoomManager.Load(rooms, true); 
    }

    if (const BlueprintResource* bp = ResBlueprint("Player/BP_Player.json").Get())
        PlayerID = bp->Instantiate(StartPlayerPos);

    DebugCamera.SetRequireHold(false); 
}

void Game::Deinit()
{
    SceneInstance.Destroy();
    Instance::Deinit();
    Physics.Deinit();
}

void Game::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    const double scaledDelta = Time.Delta();
    // TODO: Pre-update for movement logic
    Physics.Update(scaledDelta);
    ECS.Update(scaledDelta);

    if (IsKeyPressed(KEY_TAB))
    {
        bUseDebugCamera = !bUseDebugCamera;
        DebugCamera.SetReference(GetRenderScene().GetCamera());
        ECS::Input::Blocked = bUseDebugCamera; 
    }
    
    if (bUseDebugCamera)
        DebugCamera.Update(InDelta);
}

void Game::PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos)
{
    StartScene = InScene;
    StartPlayerPos = InPlayerPos;
}
