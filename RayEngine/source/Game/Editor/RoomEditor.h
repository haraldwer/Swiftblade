#pragma once

#include "RoomSubEditorManager.h"
#include "Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/Scene.h"

struct RoomEditorConfig : PropertyOwner<RoomEditorConfig>
{
    PROPERTY_D(ResScene, Scene, "Scenes/untitled.json");

    inline static const String Path = "Configs/C_RoomEditor.json";
    void LoadConfig() { Load(Path); }
    void SaveConfig() const { Save(Path); }
};

class RoomEditor : public Engine::Instance
{

public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void UpdateUI() override;
    bool IsEditor() const override { return true; }

private:
    void OpenScene();
    void PlayScene();
    void SaveRoom();
    
    // Editor stuff
    RoomEditorConfig CurrConfig;
    EditorCamera Camera;
    RoomSubEditorManager SubEditorManager; 
    
    // Scene stuff
    SceneInstance Scene;
};
