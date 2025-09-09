#pragma once

#include "Engine/ECS/Manager.h"
#include "Core/Debug/Panel.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/SceneResource.h"
#include "RoomSubEditorManager.h"
#include "History/History.h"
#include "Room/Room.h"
#include "Room/RoomResource.h"

class MenuRoomEditor;

// User preferences for the editor 
struct RoomEditorConfig : BaseConfig<RoomEditorConfig>
{
    PROPERTY_C(ResRM, Skybox, "Dressing/Skybox/RM_Skybox.json");
    PROPERTY(ResRoom, LastRoom)
    PROPERTY(String, EditMode)
    PROPERTY(Vec3F, CamPos)
    PROPERTY(Vec3F, CamRot)
    
    String Name() const override { return "RoomEditor"; }
};

class RoomEditor : public Engine::Instance, public Debug::Panel
{
    TYPE_INFO(RoomEditor, Engine::Instance);
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;

    void SetRoom(const ResRoom& InRoom);
    void LoadRoom();
    void PlayRoom();
    void SaveRoom();
    
    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Room Editor"; }
    
    bool IsEditor() const override { return true; }
    bool CanEdit() const;

    Room& GetRoom() { return workingRoom; }
    RoomSubEditorManager& GetSubEditors() { return subEditorManager; }
    Utility::History& GetHistory() { return history; }
    MenuRoomEditor& GetMenu() { CHECK_ASSERT(!menu, "Invalid menu"); return *menu; }

private:
    ResScene ConvertRoomToScene();
    
    ResRoom roomResource;
    Room workingRoom;
    
    // Editor stuff
    MenuRoomEditor* menu = nullptr;
    RoomEditorConfig config;
    RoomSubEditorManager subEditorManager; 
    
    // Scene stuff
    ECS::Manager ecs = {};
    Utility::History history = {}; // Shared history
};
