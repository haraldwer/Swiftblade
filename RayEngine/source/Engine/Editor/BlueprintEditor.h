#pragma once

#include "Blueprints/Blueprint.h"
#include "Core/Debug/Panel.h"
#include "ECS/Manager.h"
#include "Instance/Instance.h"

struct BlueprintEditorConfig : BaseConfig<BlueprintEditorConfig>
{
    PROPERTY_D(ResBlueprint, Blueprint, "Blueprints/untitled.json");
    String Name() const override { return "BlueprintEditor"; }
};

class BlueprintEditor : public Engine::Instance, public Debug::Panel
{
    friend class BlueprintDebugUI;
public:

    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;
    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Blueprint Editor"; }
    bool IsEditor() const override { return true; }
    
    void SetBP(const ResBlueprint& InBP);

private:
    void EditComponents(ECS::EntityID InID);
    void EditHierarhcy(ECS::EntityID InID);
    void HierarchyNode(ECS::EntityID InID);

    String Text(const String& InString) const;
    
    BlueprintEditorConfig Config = {};
    ECS::EntityID InstanceID = ECS::InvalidID; 
    ECS::EntityID SelectedID = ECS::InvalidID;

    ECS::Manager ECS = {};
};
