#pragma once
#include "AssetDetailPanel.h"
#include "Blueprints/Blueprint.h"

class BlueprintEditor;

class BlueprintResourceDetails : public AssetDetailPanel
{
    CLASS_INFO(BlueprintResourceDetails, AssetDetailPanel);
    
public:
    BlueprintResourceDetails(const String& InPath) : AssetDetailPanel(InPath) {}

    void Init() override;
    void Deinit() override;
    void Draw() override;
    
    bool AllowRawEdit() const override { return false; }

private:
    ResBlueprint resource;
};
