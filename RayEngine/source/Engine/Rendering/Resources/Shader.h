#pragma once
#include "Engine/Resource/Resource.h"

// Raylib shader reference
struct Shader;

class ShaderResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Shader* Get() const { return Ptr; }

private:
    String Identifier; 
    Shader* Ptr = nullptr;
    
};

typedef Resource::Ref<ShaderResource> ResShader;  