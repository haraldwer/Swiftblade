#pragma once

#include "Core/Resource/Resource.h"
#include "Texture.h"

struct RenderTexture;
struct Color;

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

class NoiseTextureResource : public PropertyOwner<NoiseTextureResource>
{
public:
    bool Load(const String& InPath) override;
    bool Unload() override;
    bool Edit(const String& InName, uint32 InOffset = 0) override;
    Utility::Timepoint GetEditTime() const;
    
    ResTexture Get() const;
    
    // Some noise properties!
    PROPERTY_D(int, Resolution, 1024);
    PROPERTY_D(float, Frequency, 1.0f);
    PROPERTY_D(int, Type, 0);
    PROPERTY_D(int, Seed, 0);

private:
    void Generate();
    void Generate(Color* InData, int InResolution);
    String GetCachePath() const;
    
    ResTexture tex = {};
    String identifier = {};
    bool editorOpen = false;
};

typedef Resource::Ref<NoiseTextureResource, true> ResNoiseTex;