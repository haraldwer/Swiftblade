﻿#pragma once

#include "Engine/ECS/Manager.h"
#include "Core/Debug/Window.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/SceneResource.h"
#include "RoomSubEditorManager.h"

struct RoomEditorConfig : BaseConfig<RoomEditorConfig>
{
    PROPERTY_D(ResScene, Scene, "Scenes/untitled.json");
    PROPERTY_D(bool, IsArena, false);
    String Name() const override { return "RoomEditor"; }
};

class RoomEditor : public Engine::Instance, public Debug::Window
{

public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;
    
    void DrawDebugWindow() override;
    String DebugWindowName() const override { return "Room Editor"; }
    
    bool IsEditor() const override { return true; }

private:
    void OpenScene();
    void PlayScene();
    void SaveRoom();
    
    // Editor stuff
    RoomEditorConfig CurrConfig;
    RoomSubEditorManager SubEditorManager; 
    
    // Scene stuff
    SceneInstance Scene;
    ECS::Manager ECS; 

    ObjectPtr<UI::Instance> UI;
};
