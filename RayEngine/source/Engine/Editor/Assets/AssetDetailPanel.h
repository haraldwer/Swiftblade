#pragma once

class AssetDetailPanel
{
    BASE_CLASS_INFO(AssetDetailPanel);

    friend class AssetDetails;
    
public:
    virtual ~AssetDetailPanel() = default;
    AssetDetailPanel(const String& InPath) : path(InPath) {}
    virtual void Init() {}
    virtual void Deinit() {}
    virtual void Draw();

    static bool Accept(const String& InPath) { return true; }

protected:
    String path;
};
