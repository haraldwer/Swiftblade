#pragma once

#include "Resource/Resource.h"
#include "Shader.h"

struct RenderTexture;

class NoiseTextureResource : public PropertyOwner<NoiseTextureResource>
{
public:
    bool Load(const String& InPath);
    bool Unload();

    bool Edit(const String& InName);
    Utility::Timepoint GetEditTime() const;
    
    Texture2D* Get() const;
    
    // Some noise properties!
    PROPERTY_D(String, Slot, "Perlin");
    PROPERTY_D(int, Resolution, 256);
    PROPERTY_D(Vec2F, InScale, Vec2F::One());

private:

    void Generate();
    void GeneratePerlin(Color* InData, int InResolution);
    
    Texture2D* Tex = nullptr;
    String Identifier;
    bool EditorOpen = false;
    
};

typedef Resource::Ref<NoiseTextureResource> ResNoiseTex;