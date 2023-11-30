#include "Game.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"
#include "Systems/Player/Input.h"

void Game::Init()
{
    Instance::Init();
    Physics.Init();

    StartScene.Unload(); // Force reload
    Vector<ResScene> rooms;
    rooms.push_back(StartScene);
    rooms.push_back(StartScene);
    rooms.push_back(StartScene);
    RoomManager.Load(rooms);

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
