#pragma once
#include "Core/Debug/Panel.h"
#include "AssetDetailPanel.h" 

namespace Editor
{
    class AssetDetails : Debug::Panel
    {
    public:
        String PanelName() const override { return "Details"; }
        void DrawPanel() override;
        void Clear();
        void Select(const Vector<String>& InPaths);
        void Deselect(const String& InPaths);

    private:
        void OpenPanel(const String& InPath);
        Vector<Object<AssetDetailPanel>> details;
    };
}
