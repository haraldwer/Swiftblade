#pragma once

#include "GameState.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/SceneResource.h"
#include "Rooms/RoomManager.h"

class GameInstance : public Engine::Instance, public Debug::Panel
{
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override; 

    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Gameplay"; }
    
    void PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos);
    void SetState(const GameState& InState);

private:

    Physics::Manager physics = {};
    ECS::Manager ecs = {}; 
    SceneInstance scene = {};
    RoomManager rooms = {};
    GameState state = {}; 
    
    ResScene startScene = {};
    Vec3F startPlayerPos = {};
};
