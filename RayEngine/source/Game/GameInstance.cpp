#include "GameInstance.h"

#include <filesystem>

#include "ECS/Player/PlayerInput.h"
#include "ECS/Player/Animation/Animator.h"
#include "ECS/Player/Combat/Combat.h"
#include "ECS/Player/Movement/Movement.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"

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
        bp->Instantiate(StartPlayerPos + Vec3F::Up() * 2.0f, {}, State.PlayerID);
    }
}

void GameInstance::Deinit()
{
    State.PlayerID = ECS::InvalidID;
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

void GameInstance::DrawDebugWindow()
{
    CHECK_RETURN(State.PlayerID == ECS::InvalidID);
    
    if (ImGui::CollapsingHeader("Movement"))
        if (auto m = ECS.GetComponent<ECS::Movement>(State.PlayerID))
            m->EditState();
    if (ImGui::CollapsingHeader("Combat"))
        if (auto c = ECS.GetComponent<ECS::Combat>(State.PlayerID))
            c->EditState();
    if (ImGui::CollapsingHeader("Animation"))
        if (auto a = ECS.GetComponent<ECS::Animator>(State.PlayerID))
            a->EditState();
}