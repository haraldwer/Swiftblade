#pragma once

#include "Core/Resource/Resource.h"
#include "Resources/Shader.h"

struct Font;

class FontResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Font* Get(uint32 InSize);
    Shader* GetShader() const;
    
    bool Save(const String& InPath) { return false; }
    bool Edit(const String& InName);

private:
    ResShader sdfShader;
    String identifier = {}; 
    Map<uint32, Font*> sizes;  
};

typedef Resource::Ref<FontResource, true> ResFont;  
