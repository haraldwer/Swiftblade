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
        bool Load() override;
        Utility::Timepoint GetEditTime() const override;
        ResShader GetShader() const;
        uint32 Hash() const { return hash; }

        static bool Accept(const String& InPath);
        
    private:
        uint32 hash = 0;
    };
}

typedef Resource::Ref<Rendering::MaterialResource> ResRM;  
