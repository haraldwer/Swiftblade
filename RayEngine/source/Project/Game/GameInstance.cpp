#include "GameInstance.h"

#include <filesystem>

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"
#include "ECS/Player/PlayerInput.h"

void GameInstance::Init()
{
    Instance::Init();
    ECS.Init();
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
        RoomManager.LoadConfig();  
    }

    if (const BlueprintResource* bp = ResBlueprint("Gameplay/Player/BP_Player.json").Get())
    { 
        State.PlayerID = ECS.CreateEntity(); 
        bp->Instantiate(StartPlayerPos, {}, State.PlayerID);
    }

    DebugCamera.SetRequireHold(false); 
}

void GameInstance::Deinit()
{
    SceneInstance.Destroy();
    ECS.Deinit(); 
    Physics.Deinit();
    Instance::Deinit();
}

void GameInstance::Logic(double InDelta)
{
    Instance::Logic(InDelta); 
    
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
        ECS::PlayerInput::Blocked = bUseDebugCamera; 
    }
    
    if (bUseDebugCamera)
        DebugCamera.Update(InDelta); 
}

void GameInstance::Frame(double InDelta)
{
    Instance::Frame(InDelta);
}

void GameInstance::PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos)
{
    StartScene = InScene;
    StartPlayerPos = InPlayerPos;
}

void GameInstance::SetState(const GameState& InState)
{
    const ECS::EntityID playerID = State.PlayerID;
    State = InState;
    State.PlayerID = playerID; 
}
