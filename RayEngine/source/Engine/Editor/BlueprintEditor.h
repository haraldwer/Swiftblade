#pragma once

#include "Blueprints/Blueprint.h"
#include "Core/Debug/Panel.h"
#include "ECS/Manager.h"
#include "Instance/Instance.h"

struct BlueprintEditorConfig : BaseConfig<BlueprintEditorConfig>
{
    PROPERTY_D(ResBlueprint, Blueprint, "Blueprints/untitled.json");
    PROPERTY_D(ResRM, DefaultSkybox, "Dressing/Skybox/RM_Skybox.json");
    String Name() const override { return "BlueprintEditor"; }
};

class BlueprintEditor : public Engine::Instance, public Debug::Panel
{
    CLASS_INFO(BlueprintEditor, Engine::Instance);
    
    friend class BlueprintDebugUI;
    
public:
    void Init() override;
    void Deinit() override;
    void Logic(double InDelta) override;
    void Frame() override;
    void DrawDebugPanel() override;
    String DebugPanelName() const override { return "Blueprint Editor"; }
    bool IsEditor() const override { return true; }
    
    void SetPendingBP(const ResBlueprint& InBP);
    void SetBP(const ResBlueprint& InBP);

private:
    
    ResBlueprint pendingBP; 
    BlueprintEditorConfig config = {};
    ECS::EntityID instanceID = ECS::INVALID_ID; 
    ECS::EntityID selectedID = ECS::INVALID_ID;

    ECS::Manager ecs = {};
};
