#pragma once

#include "Resource/PropertyFile.h"
#include "Resources/Shader.h"
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    struct BakedTexData : PropertyOwner<BakedTexData>
    {
        PROPERTY(ResShader, Shader);
        PROPERTY_D(String, Name, "TexBaked");
        PROPERTY_D(int, Res, 1024);
    };
    
    class BakedTexture : public Resource::PropertyFile<BakedTexData>
    {
        friend class Renderer;
        
    public:
        bool Load() override;
        bool Unload() override;
        Utility::Timepoint GetEditTime() const override;
        
        bool Bake();
        bool IsBaked() const { return baked; }
        RenderTarget& Get() { return target; }
    
    private:
        RenderTarget target;
        bool baked = false;
    };
    
}

typedef Resource::Ref<Rendering::BakedTexture> ResBakedTex;  
