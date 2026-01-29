#pragma once

#include "Blueprints/Blueprint.h"
#include "Core/Debug/Panel.h"
#include "ECS/Manager.h"
#include "Instance/Instance.h"
#include "Resources/Material.h"

namespace Editor
{
    struct BlueprintEditorConfig : BaseConfig<BlueprintEditorConfig>
    {
        PROPERTY_D(ResBlueprint, Blueprint, "Blueprints/untitled.json");
        PROPERTY_D(ResRM, DefaultSkybox, "Dressing/Skybox/RM_Skybox.json");
        PROPERTY_D(bool, Autosave, true);
        String Name() const override { return "BlueprintEditor"; }
    };

    class BlueprintEditor : public Engine::Instance, public Debug::Panel
    {
        CLASS_INFO(BlueprintEditor, Engine::Instance);
        
        friend class BlueprintDebugUI;
        
    public:
        void Init() override;
        void Deinit() override;
        void Tick(double InDelta) override;
        void Frame() override;
        void DrawPanel() override;
        String PanelName() const override { return "Blueprint Editor"; }
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
}
