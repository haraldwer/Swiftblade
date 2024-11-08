#include "GameInstance.h"

#include <filesystem>

#include "Engine/Blueprints/Blueprint.h"
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
    
    if (!Time.IsPaused())
        Physics.Update();
    ECS.Update();
    EditorCamera.Update(); 
}

void GameInstance::Frame()
{
    ECS.Frame(); 
    Instance::Frame();
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
