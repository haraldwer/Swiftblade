#pragma once
#include "AssetDetailPanel.h"

namespace Editor
{
    class TextDetails : public AssetDetailPanel
    {
        CLASS_INFO(TextDetails, AssetDetailPanel);
    public:
        TextDetails(const String& InPath) : AssetDetailPanel(InPath) {}
        void Init() override;
        void Draw() override;
        bool AllowRawEdit() const override { return false;}
        static bool Accept(const String& InPath);
    };
    
}
