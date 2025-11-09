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
        if (config.Edit())
            config.Save(path);
    }
    
    static bool Accept(const String& InPath)
    {
        return InPath.contains(T().Path());
    }

private:
    T config;
};
