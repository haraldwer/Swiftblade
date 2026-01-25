#include "GameInstance.h"

#include "../ECS/Player/Animation/PlayerAnimator.h"
#include "../ECS/Player/Combat/Combat.h"
#include "../ECS/Player/Movement/Movement.h"
#include "ECS/Registration.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/SceneResource.h"
#include "Game/ECS/Registration.h"
#include "ImGui/imgui.h"
#include "Instance/InstanceManager.h"
#include "UI/Menus/MenuPause.h"

void GameInstance::Init()
{
    Instance::Init();
    
    ECS::RegisterEngineSystems();
    ECS::RegisterGameSystems();
    ecs.Init();
    physics.Init();

    CHECK_ASSERT(state.playerID != ECS::INVALID_ID, "No player should exist at this point");
    
    if (startScene.scene.Identifier().IsValid() && startScene.repeats > 0)
    {
        Vector<ResScene> roomList;
        for (int i = 0; i < startScene.repeats; i++)
            roomList.push_back(startScene.scene);
        if (!startScene.scene.Identifier().Unique())
            startScene.scene.Unload(); // Force reload
        rooms.Load(roomList);
    }
    else
    {
        rooms.LoadLevel(state.level);  
    }

    if (state.playerID == ECS::INVALID_ID)
    {
        LOG("No player spawned, creating default player")
        if (const BlueprintResource* bp = ResBlueprint("Gameplay/Player/BP_Player.json").Get())
        { 
            state.playerID = ecs.CreateEntity(); 
            bp->Instantiate(startScene.playerPos + Vec3F::Up() * 2.0f, {}, state.playerID);
        }
    }
}

void GameInstance::Deinit()
{
    state.playerID = ECS::INVALID_ID;
    scene.Destroy();
    ecs.Deinit(); 
    physics.Deinit();
    Instance::Deinit();
}

void GameInstance::Logic(const double InDelta)
{
    Instance::Logic(InDelta); 
    
    if (!time.IsPaused())
    {
        physics.Update();
        if (Input::Action::Get("Back").Pressed())
            Menu::Manager::Get().Push<MenuPause>();
    }
    ecs.Update();
    freeCamera.Update();
}

void GameInstance::Frame()
{
    ecs.Frame();
    physics.Frame();
    Instance::Frame();
}

void GameInstance::PlayScene(const StartSceneParams& InParams)
{
    startScene = InParams;
}

void GameInstance::PlayLevel(const LevelConfig& InLevel)
{
    GameState newState;
    newState.level = InLevel;
    SetState(newState);
}

void GameInstance::SetState(const GameState& InState)
{
    const ECS::EntityID playerID = state.playerID;
    state = InState;
    state.playerID = playerID; 
}

#ifdef IMGUI_ENABLE

void GameInstance::DrawPanel()
{
    CHECK_RETURN(state.playerID == ECS::INVALID_ID);

    static float timeScale = 1.0f;
    Utility::Edit("TimeScale", timeScale);
    if (timeScale > 0.01f)
        Utility::Time::Get().SetScale(timeScale);
    
    if (ImGui::CollapsingHeader("Movement"))
        if (const auto m = ecs.GetComponent<ECS::Movement>(state.playerID))
            m->EditState();
    if (ImGui::CollapsingHeader("Combat"))
        if (const auto c = ecs.GetComponent<ECS::Combat>(state.playerID))
            c->EditState();
    if (ImGui::CollapsingHeader("Animation"))
        if (const auto a = ecs.GetComponent<ECS::PlayerAnimator>(state.playerID))
            a->EditState();
}

#else
void GameInstance::DrawDebugPanel() {  }
#endif