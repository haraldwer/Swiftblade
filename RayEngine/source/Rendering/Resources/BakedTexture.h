#pragma once

#include "Resources/Shader.h"
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    class BakedTexture : public PropertyOwner<BakedTexture>
    {
        friend class Renderer;
    public:
        bool Load(const String& InPath) override;
        bool Unload() override;
        Utility::Timepoint GetEditTime() const;
        bool Bake();
        bool IsBaked() const { return baked; }
        RenderTarget& Get() { return target; };
        
        PROPERTY(ResShader, Shader);
        PROPERTY_D(String, Name, "TexBaked");
        PROPERTY_D(int, Res, 1024);
        
    private:
        
        RenderTarget target;
        RenderTexture* tex = nullptr;
        bool baked = false;

        
    };
}

typedef Resource::Ref<Rendering::BakedTexture, true> ResBakedTex;  