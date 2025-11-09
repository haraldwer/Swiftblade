#pragma once
#include "Debug/Panel.h"
#include "AssetDetailPanel.h" 

class AssetDetails : Debug::Panel
{
public:
    String DebugPanelName() const override { return "Details"; }
    void DrawDebugPanel() override;
    void Clear();
    void Select(const Vector<String>& InPaths);
    void Deselect(const String& InPaths);

private:
    void OpenPanel(const String& InPath);
    Vector<Object<AssetDetailPanel>> details;
};
