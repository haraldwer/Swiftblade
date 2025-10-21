#pragma once

#include "Resource/PropertyFile.h"
#include "Resources/Shader.h"

struct RenderTexture;

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
        
        void Bind(const String& InName, ShaderResource& InShader, int& InOutSlot, int InFilter) const;
        bool Bake();
        bool IsBaked() const { return baked; }
        RenderTexture* Get() const { return tex; }

    private:
        RenderTexture* tex = nullptr;
        bool baked = false;
    };
    
}

typedef Resource::Ref<Rendering::BakedTexture> ResBakedTex;  
