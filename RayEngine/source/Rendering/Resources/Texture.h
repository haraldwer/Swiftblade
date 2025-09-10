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
    Texture* Get() const { return ptr; }

    bool Edit(const String& InName, uint32 InOffset = 0) const;
    bool Save(const String& InPath) { return false; }
    
private:
    String identifier = {}; 
    Texture* ptr = nullptr;
};

typedef Resource::Ref<TextureResource> ResTexture;