#pragma once

#include "GameState.h"
#include "Rooms/RoomManager.h"
#include "Engine/Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"

class GameInstance : public Engine::Instance 
{
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override; 
    
    void PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos);
    void SetState(const GameState& InState);

private:

    Physics::Manager Physics;
    ECS::Manager ECS; 
    SceneInstance SceneInstance;
    RoomManager RoomManager;
    GameState State; 
    
    ResScene StartScene;
    Vec3F StartPlayerPos; 
};
