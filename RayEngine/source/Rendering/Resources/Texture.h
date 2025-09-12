#pragma once

#include "Core/Resource/Resource.h"

// Raylib model reference
struct Texture; 

namespace Rendering
{
    class TextureResource : Resource::Base
    {
        friend class NoiseTextureResource;
        CLASS_INFO(TextureResource, Resource::Base)
    public:
        bool Load() override;
        bool Unload() override;
        bool Edit(const String& InName, uint32 InOffset = 0) override;
        Utility::Timepoint GetEditTime() const override;
        Texture* Get() const { return ptr; }
        
    private:
        Texture* ptr = nullptr;
    };
}

typedef Resource::Ref<Rendering::TextureResource> ResTexture;
