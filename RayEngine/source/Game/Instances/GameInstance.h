#pragma once

#include "../GameState.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/SceneResource.h"
#include "../Level/LevelManager.h"
#include "ECS/Manager.h"
#include "Game/Level/Level.h"

class GameInstance : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(GameInstance, Engine::Instance);
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override; 

    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Gameplay"; }
    
    void PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos);
    void PlayLevel(const LevelConfig& InLevel);
    void SetState(const GameState& InState);

private:

    Physics::Manager physics = {};
    ECS::Manager ecs = {}; 
    SceneInstance scene = {};
    LevelManager rooms = {};
    GameState state = {}; 
    
    ResScene startScene = {};
    Vec3F startPlayerPos = {};
};
