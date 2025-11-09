#pragma once
#include "AssetDetailPanel.h"
#include "Resources/Texture.h"

template <class T>
class ResourceDetails : public AssetDetailPanel
{
    CLASS_INFO(ResourceDetails, AssetDetailPanel);
    
public:
    ResourceDetails(const String& InPath) : AssetDetailPanel(InPath), resource(InPath) {}

    void Draw() override
    {
        ImGui::Text("Resource: %s", Utility::Filename(path).c_str());
    
        auto res = resource.Get();
        if (!res)
        {
            ImGui::Text("Failed to get resource");
            return;
        }

        // Use the existing edit function for resources
        res->Edit("Details##" + path);
    }
    
    static bool Accept(const String& InPath)
    {
        return T::EditAccept(InPath);
    }
    
private:
    Resource::Ref<T> resource;
};
