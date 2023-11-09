#pragma once

#include "Editor/EditorCamera.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Scene/Scene.h"

class RoomEditor : public Engine::Instance
{
    struct Config : PropertyOwner<Config>
    {
        PROPERTY_P(ResScene, Scene, "Scenes/untitled.json");

        static constexpr String Path = "Configs/C_RoomEditor.json";
        void Load() { PropertyOwner::Load(Path); }
        void Save() const { PropertyOwner::Save(Path); }
    };
    
public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void UpdateUI() override;
    bool IsEditor() const override { return true; }

private:
    Config CurrConfig;
    EditorCamera Camera;
    SceneInstance Scene;
};
