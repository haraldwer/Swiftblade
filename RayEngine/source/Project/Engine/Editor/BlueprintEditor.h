#pragma once
#include "EditorCamera.h"
#include "DebugWindow/DebugUI.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Instance.h"

struct BlueprintEditorConfig : PropertyOwner<BlueprintEditorConfig>
{
    PROPERTY_D(ResBlueprint, Blueprint, "Blueprints/untitled.json");

    inline static const String Path = "Configs/C_BlueprintEditor.json";
    void LoadConfig() { Load(Path); }
    void SaveConfig() const { Save(Path); }
};

class BlueprintEditor : public Engine::Instance, public DebugUI
{
    friend class BlueprintDebugUI;
public:

    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void Draw() override;
    void DrawDebugUI() override;
    bool IsEditor() const override { return true; }
    
    void SetBP(const ResBlueprint& InBP);

private:

    void EditComponents(ECS::EntityID InID);
    void EditHierarhcy(ECS::EntityID InID);
    void HierarchyNode(ECS::EntityID InID);

    String Text(const String& InString) const;
    
    EditorCamera Camera;
    BlueprintEditorConfig Config;
    ECS::EntityID InstanceID = ECS::InvalidID; 
    ECS::EntityID SelectedID = ECS::InvalidID;  
};
