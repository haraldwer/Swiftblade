#pragma once

#include "Core/Resource/Resource.h"
#include "Resources/Shader.h"
#include "Resources/Texture.h"

struct Font;

class FontResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Font* Get(uint32 InSize);
    Shader* GetShader() const;
    
    bool Edit(const String& InName, uint32 InOffset = 0);
    bool Save(const String& InPath) { return false; }

private:

    String GetCachePath(int InSize) const;
    ResShader sdfShader = ResShader("Shaders/SH_FontSDF.fs");
    String identifier = {};
    
    Map<uint32, Font*> sizes;

    static constexpr uint32 maxSize = 40;
};

typedef Resource::Ref<FontResource, true> ResFont;  
