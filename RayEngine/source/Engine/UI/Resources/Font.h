#pragma once

#include "Core/Resource/Resource.h"

struct Font;

class FontResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Font* Get() const { return ptr; }
    
    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName) { return false; };

private:
    Font* ptr = nullptr;
    String identifier = {}; 
};

typedef Resource::Ref<FontResource, true> ResFont;  
