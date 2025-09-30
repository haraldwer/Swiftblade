#pragma once

#include "Core/Resource/Resource.h"
#include "Resources/Shader.h"
#include "Resources/Texture.h"

struct Font;

namespace UI
{
    class FontResource : public Resource::Base
    {
    public:
        bool Load() override;
        bool Unload() override; 
        bool Edit(const String& InName, uint32 InOffset = 0) override;
        
        Font* Get(uint32 InSize);
        Shader* GetShader() const;

    private:
        String GetCachePath(int InSize) const;
        
        ResShader sdfShader = ResShader("Shaders/SH_FontSDF.fs");
        Map<uint32, Font*> sizes;
        static constexpr uint32 maxSize = 40;
        static constexpr uint32 minSize = 5;
    };
}

typedef Resource::Ref<UI::FontResource> ResFont;  
