#pragma once

#include "RoomManager.h"
#include "Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"

class Game : public Engine::Instance 
{
    
public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void PlayScene(const ResScene& InScene, const Vec3F& InPlayerPos);

private:

    Physics::Manager Physics;
    SceneInstance SceneInstance;

    bool bUseDebugCamera = false; 
    EditorCamera DebugCamera;

    ECS::EntityID PlayerID;
    RoomManager RoomManager;
    
    ResScene StartScene = ResScene("Scenes/RM_Scene.json");
    Vec3F StartPlayerPos; 
};
