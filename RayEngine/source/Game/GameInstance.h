#pragma once

#include "RoomManager.h"
#include "Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"

class GameInstance : public Engine::Instance 
{
    
public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void UpdateUI() override;
    void PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos);

private:

    Physics::Manager Physics;
    SceneInstance SceneInstance;

    bool bUseDebugCamera = false; 
    EditorCamera DebugCamera;

    ECS::EntityID PlayerID;
    RoomManager RoomManager;
    
    ResScene StartScene;
    Vec3F StartPlayerPos; 
};
