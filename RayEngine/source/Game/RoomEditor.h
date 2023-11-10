#pragma once

#include "Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/Scene.h"

struct RoomEditorConfig : PropertyOwner<RoomEditorConfig>
{
    PROPERTY_P(ResScene, Scene, "Scenes/untitled.json");

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
    RoomEditorConfig CurrConfig;
    EditorCamera Camera;
    SceneInstance Scene;
    ECS::EntityID CubeVolume = ECS::InvalidID;
    
    float PlaceDist = 5.0f;
    uint32 PlaceStart = 0;
    bool Placing = false; 
};
