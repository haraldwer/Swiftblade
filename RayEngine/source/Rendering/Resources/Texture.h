#pragma once

#include "Core/Resource/Resource.h"

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
        uint32 Get() const { return texture; }
        static bool Accept(const String& InPath);

    private:
        uint32 texture = 0;
    };
}

typedef Resource::Ref<Rendering::TextureResource> ResTexture;
