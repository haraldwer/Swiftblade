#pragma once
#include "EditorCamera.h"
#include "Debugging\Window.h"
#include "..\BaseConfig.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Instance.h"

struct BlueprintEditorConfig : BaseConfig<BlueprintEditorConfig>
{
    PROPERTY_D(ResBlueprint, Blueprint, "Blueprints/untitled.json");
    String Name() const override { return "BlueprintEditor"; }
};

class BlueprintEditor : public Engine::Instance, public Debug::Window
{
    friend class BlueprintDebugUI;
public:

    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;
    void Draw() override;
    void DrawDebugWindow() override;
    String DebugWindowName() const override { return "Blueprint Editor"; }
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

    ECS::Manager ECS; 
};
