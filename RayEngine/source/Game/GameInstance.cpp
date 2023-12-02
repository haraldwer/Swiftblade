#include "GameInstance.h"

#include <filesystem>

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"
#include "ECS/Player/Input.h"

void GameInstance::Init()
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

void GameInstance::Deinit()
{
    SceneInstance.Destroy();
    Instance::Deinit();
    Physics.Deinit();
}

void GameInstance::Update(double InDelta)
{
    Time.Tick(InDelta);
    
    // Update
    const double scaledDelta = Time.Delta();
    // TODO: Pre-update for movement logic
    if (!Time.IsPaused())
        Physics.Update(scaledDelta);
    ECS.Update(scaledDelta);
    Menus.Update(scaledDelta);
    
    if (IsKeyPressed(KEY_TAB))
    {
        bUseDebugCamera = !bUseDebugCamera;
        DebugCamera.SetReference(GetRenderScene().GetCamera());
        ECS::Input::Blocked = bUseDebugCamera; 
    }
    
    if (bUseDebugCamera)
        DebugCamera.Update(InDelta); 
}

void GameInstance::UpdateUI()
{
    ECS.UpdateUI();
    Menus.Draw(); 
}

void GameInstance::PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos)
{
    StartScene = InScene;
    StartPlayerPos = InPlayerPos;
}
