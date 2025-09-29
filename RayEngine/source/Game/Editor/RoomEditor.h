#pragma once

#include "Engine/ECS/Manager.h"
#include "Core/Debug/Panel.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/SceneResource.h"
#include "RoomSubEditorManager.h"
#include "History/History.h"
#include "EditRoom.h"

class MenuRoomEditor;

// User preferences for the editor 
struct RoomEditorConfig : BaseConfig<RoomEditorConfig>
{
    PROPERTY_C(ResRM, Skybox, "Dressing/Skybox/RM_Skybox.json");
    PROPERTY(ResEditRoom, LastRoom)
    PROPERTY(String, EditMode)
    PROPERTY(Vec3F, CamPos)
    PROPERTY(Vec3F, CamRot)
    
    String Name() const override { return "RoomEditor"; }
};

class RoomEditor : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(RoomEditor, Engine::Instance);
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;

    void SetRoom(const ResEditRoom& InRoom);
    void LoadRoom();
    void PlayRoom();

    void SaveRoom();
    bool SubmitRoom();
    
    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Room Editor"; }
    
    bool IsEditor() const override { return true; }
    bool CanEdit() const;

    EditRoom& GetRoom() { return workingRoom; }
    RoomSubEditorManager& GetSubEditors() { return subEditorManager; }
    Utility::History& GetHistory() { return history; }
    MenuRoomEditor& GetMenu() { CHECK_ASSERT(!menu, "Invalid menu"); return *menu; }

private:
    
    void UpdateRoomInfo();
    SceneInstance ConvertRoomToScene();
    static ResScene GetTempScene(const SceneInstance& InScene);
    static void SaveThumbnail(const String& InPath);
    
    ResEditRoom roomResource;
    EditRoom workingRoom;
    
    // Editor stuff
    MenuRoomEditor* menu = nullptr;
    RoomEditorConfig config;
    RoomSubEditorManager subEditorManager; 
    
    // Scene stuff
    ECS::Manager ecs = {};
    Utility::History history = {}; // Shared history
};
