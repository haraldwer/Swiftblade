#pragma once
#include "Shader.h"

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
        bool Edit(const String &InName = "", uint32 InOffset = 0) override;
        Utility::Timepoint GetEditTime() const override;
        
        bool Bake();
        bool IsBaked() const { return baked; }

        static bool Accept(const String& InPath);
        
    private:
        bool baked = false;
    };
    
}

typedef Resource::Ref<Rendering::BakedTexture> ResBakedTex;  
