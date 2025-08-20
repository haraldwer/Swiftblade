#pragma once

#include "Core/Resource/Resource.h"

struct Font;

class FontResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Font* Get(uint32 InSize);
    
    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName) { return false; };

private:
    String identifier = {}; 
    Map<uint32, Font*> sizes;
};

typedef Resource::Ref<FontResource, true> ResFont;  
