#pragma once

#include "Shader.h"

namespace Rendering
{
    struct MaterialData : PropertyOwner<MaterialData>
    {
        PROPERTY_D(ResShader, Shader, "Shaders/SH_Default.sh");
        
        // Also blending and other stuff!
        //PROPERTY_D(bool, TwoSided, false);
        //PROPERTY_D(bool, Transparent, false);
        // 
    };

    class MaterialResource : public Resource::PropertyFile<MaterialData>
    {
        CLASS_INFO(MaterialResource, Resource::PropertyFile<MaterialData>)
        
    public:
        Utility::Timepoint GetEditTime() const override;
        ResShader GetShader() const;

        static bool Accept(const String& InPath);
    };
}

typedef Resource::Ref<Rendering::MaterialResource> ResRM;  
