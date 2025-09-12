#pragma once

#include "Resource/Resource.h"
#include "Resource/PropertyFile.h"
#include "Texture.h"

struct RenderTexture;
struct Color;

namespace Rendering
{
    enum class NoiseType : uint8
    {
        OPEN_SIMPLEX_2,
        OPEN_SIMPLEX_2S,
        CELLULAR,
        PERLIN,
        VALUE_CUBIC,
        VALUE,
        COUNT
    };

    struct NoiseData : PropertyOwner<NoiseData>
    {
        PROPERTY_D(int, Resolution, 1024);
        PROPERTY_D(float, Frequency, 1.0f);
        PROPERTY_D(int, Type, 0);
        PROPERTY_D(int, Seed, 0);
    };
    
    class NoiseTextureResource : public Resource::PropertyFile<NoiseData>
    {
        CLASS_INFO(NoiseTextureResource, Resource::PropertyFile<NoiseData>)
        
    public:
        bool Load() override;
        bool Unload() override;
        bool Edit(const String& InName, uint32 InOffset = 0) override;
        ResTexture Get() const;
        
    private:
        void Generate();
        void Generate(Color* InData, int InResolution);
        String GetCachePath() const;
        
        ResTexture tex = {};
        bool editorOpen = false;
    };
}

typedef Resource::Ref<Rendering::NoiseTextureResource> ResNoiseTex;
