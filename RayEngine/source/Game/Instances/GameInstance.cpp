#include "GameInstance.h"

#include <filesystem>

#include "../ECS/Player/PlayerInput.h"
#include "../ECS/Player/Animation/Animator.h"
#include "../ECS/Player/Combat/Combat.h"
#include "../ECS/Player/Movement/Movement.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/SceneResource.h"
#include "ImGui/imgui.h"
#include "Instance/Manager.h"
#include "UI/Menus/MenuPause.h"

void GameInstance::Init()
{
    Instance::Init();
    ecs.Init();
    physics.Init();

    if (startScene.Identifier().IsValid())
    {
        Vector<ResScene> roomList;
        roomList.push_back(startScene);
        roomList.push_back(startScene);
        roomList.push_back(startScene);
        if (!startScene.Identifier().Unique())
            startScene.Unload(); // Force reload
        rooms.Load(roomList, false);
    }
    else
    {
        rooms.LoadLevel(level);  
    }

    if (const BlueprintResource* bp = ResBlueprint("Gameplay/Player/BP_Player.json").Get())
    { 
        state.playerID = ecs.CreateEntity(); 
        bp->Instantiate(startPlayerPos + Vec3F::Up() * 2.0f, {}, state.playerID);
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
    editorCamera.Update();
}

void GameInstance::Frame()
{
    ecs.Frame();
    physics.Frame();
    Instance::Frame();
}

void GameInstance::PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos)
{
    startScene = InScene;
    startPlayerPos = InPlayerPos;
}

void GameInstance::PlayLevel(const ::LevelConfig &InLevel)
{
    level = InLevel;
}

void GameInstance::SetState(const GameState& InState)
{
    const ECS::EntityID playerID = state.playerID;
    state = InState;
    state.playerID = playerID; 
}

void GameInstance::DrawDebugPanel()
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
        if (const auto a = ecs.GetComponent<ECS::Animator>(state.playerID))
            a->EditState();
}