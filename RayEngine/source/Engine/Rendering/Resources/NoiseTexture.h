#pragma once

#include "Resource/Resource.h"
#include "Texture.h"

struct RenderTexture;

enum class NoiseType : uint8
{
    PERLIN = 0,
    VORONOI,
    COUNT
};

struct PerlinProperties : PropertyOwner<PerlinProperties>
{
    PROPERTY_D(float, Lacunarity, 2.0f);
    PROPERTY_D(float, Gain, 0.5f);
    PROPERTY_D(int, Octaves, 6);
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
    PROPERTY_D(Vec2F, Scale, Vec2F::One());
    PROPERTY_D(int, WarpDepth, 0);
    PROPERTY_D(int, Type, 0);
    PROPERTY(PerlinProperties, Perlin);

private:
    Texture*& GetTex() const;
    void Generate();
    void GeneratePerlin(Color* InData, int InResolution);
    
    ResTexture Tex;
    String Identifier;
    bool EditorOpen = false;
};

typedef Resource::Ref<NoiseTextureResource, true> ResNoiseTex;