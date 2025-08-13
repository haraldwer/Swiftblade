#pragma once

#include "Engine/ECS/Manager.h"
#include "Core/Debug/Panel.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/SceneResource.h"
#include "RoomSubEditorManager.h"
#include "History/History.h"
#include "Room/Room.h"

class MenuRoomEditor;

struct RoomEditorConfig : BaseConfig<RoomEditorConfig>
{
    PROPERTY_D(ResScene, Scene, "Scenes/untitled.json");
    PROPERTY_D(bool, IsArena, false);
    PROPERTY_D(ResRM, Skybox, "Dressing/Skybox/RM_Skybox.json");
    String Name() const override { return "RoomEditor"; }
};

class RoomEditor : public Engine::Instance, public Debug::Panel
{
    friend MenuRoomEditor;
    
    TYPE_INFO(RoomEditor, Engine::Instance);
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;
    
    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Room Editor"; }
    
    bool IsEditor() const override { return true; }
    RoomSubEditorManager& GetSubEditors() { return subEditorManager; }
    Utility::History& GetHistory() { return history; }
    
private:
    void OpenScene();
    void PlayScene();
    void SaveRoom();
    
    // Editor stuff
    MenuRoomEditor* menu = nullptr;
    RoomEditorConfig currConfig = {};
    RoomSubEditorManager subEditorManager; 
    
    // Scene stuff
    SceneInstance scene = {};
    ECS::Manager ecs = {};
    Utility::History history = {}; // Shared history
};
