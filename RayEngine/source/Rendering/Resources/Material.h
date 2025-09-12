#pragma once

#include "Rendering/Resources/Shader.h"
#include "Core/Resource/Resource.h"
#include "Resource/PropertyFile.h"

namespace Rendering
{
    struct MaterialData : PropertyOwner<MaterialData>
    {
        PROPERTY_D(ResShader, SurfaceShader, "Shaders/SH_Default");
        PROPERTY_D(ResShader, DeferredShader, "Shaders/SH_Default.ds");
        PROPERTY_D(bool, TwoSided, false);
        PROPERTY_D(bool, Transparent, false);
    };

    class MaterialResource : public Resource::PropertyFile<MaterialData>
    {
        CLASS_INFO(MaterialResource, Resource::PropertyFile<MaterialData>)
        
    public:
        bool Load() override;
        Utility::Timepoint GetEditTime() const override;
        
        uint32 Hash();
        uint32 DeferredHash();

    private:
        uint32 cachedHash = 0;
        uint32 cachedDeferredHash = 0;
    };
}

typedef Resource::Ref<Rendering::MaterialResource> ResRM;  
