#pragma once

#include "Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"
#include "Menus/MenuMain.h"

class Game : public Engine::Instance 
{
    
public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void UpdateUI() override;

private:

    Physics::Manager Physics;
    SceneInstance SceneInstance;

    bool bUseDebugCamera = false; 
    EditorCamera DebugCamera;

    ECS::EntityID PlayerID;

    MenuMain Menu; 
};
