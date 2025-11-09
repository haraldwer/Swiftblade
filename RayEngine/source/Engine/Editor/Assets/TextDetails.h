#pragma once
#include "AssetDetailPanel.h"

class TextDetails : public AssetDetailPanel
{
    CLASS_INFO(TextDetails, AssetDetailPanel);
public:
    TextDetails(const String& InPath) : AssetDetailPanel(InPath) {}
    void Init() override;
    void Draw() override;
    static bool Accept(const String& InPath);
    
private:
    String content;
};
