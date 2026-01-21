#pragma once
#include "AssetDetailPanel.h"

template <class T>
class ConfigDetails : public AssetDetailPanel
{
    CLASS_INFO(ConfigDetails, AssetDetailPanel);
    
public:
    ConfigDetails(const String& InPath) : AssetDetailPanel(InPath) {}

    void Init() override
    {
        config.Load(path);
    }
    
    void Draw() override
    {
        DrawFileInfo();
        if (config.Edit())
            config.Save(path);
    }

    bool AllowRawEdit() const override { return true; }
    
    static bool Accept(const String& InPath)
    {
        return InPath.ends_with(T().Path());
    }

private:
    T config;
};
