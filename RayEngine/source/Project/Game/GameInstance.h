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
    void Update(double InDelta) override;
    void Draw() override;
    
    void PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos);
    void SetState(const GameState& InState);

private:

    Physics::Manager Physics;
    SceneInstance SceneInstance;
    RoomManager RoomManager;
    GameState State; 

    bool bUseDebugCamera = false; 
    EditorCamera DebugCamera;
    
    ResScene StartScene;
    Vec3F StartPlayerPos; 
};
