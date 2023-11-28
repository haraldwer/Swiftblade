#pragma once

#include "Engine/Resource/Resource.h"

// Raylib model reference
struct Texture; 

class TextureResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const;
    Texture* Get() const { return Ptr; }

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName) { return false; };
    
private:
    String Identifier; 
    Texture* Ptr = nullptr; 
};

typedef Resource::Ref<TextureResource> ResTexture;  