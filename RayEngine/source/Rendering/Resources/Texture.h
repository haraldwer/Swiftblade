#pragma once

#include "Core/Resource/Resource.h"

// Raylib model reference
struct Texture; 

class TextureResource
{
    friend class NoiseTextureResource;
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const;
    Texture* Get() const { return Ptr; }

private:
    String Identifier; 
    Texture* Ptr = nullptr;
};

typedef Resource::Ref<TextureResource> ResTexture;  