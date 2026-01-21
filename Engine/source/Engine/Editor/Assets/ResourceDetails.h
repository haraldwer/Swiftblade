#pragma once
#include "AssetDetailPanel.h"
#include "ImGui/imgui.h"

template <class T, bool RawEdit>
class ResourceDetails : public AssetDetailPanel
{
    CLASS_INFO(ResourceDetails, AssetDetailPanel);
    
public:
    ResourceDetails(const String& InPath) : AssetDetailPanel(InPath), resource(InPath) {}

    void Draw() override
    {
        DrawFileInfo();
    
        auto res = resource.Get();
        if (!res)
        {
            ImGui::Text("Failed to get resource");
            return;
        }

        // Use the existing edit function for resources
        res->Edit("Details##" + path);
    }
    
    bool AllowRawEdit() const override { return RawEdit; }
    
    static bool Accept(const String& InPath)
    {
        return T::Accept(InPath);
    }
    
private:
    Resource::Ref<T> resource;
};

template <class T>
using TextResourceDetails = ResourceDetails<T, true>;

template <class T>
using FileResourceDetails = ResourceDetails<T, false>;
