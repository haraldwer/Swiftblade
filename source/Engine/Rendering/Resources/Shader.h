#pragma once
#include "Engine/Resource/Resource.h"

// Raylib shader reference
struct Shader;

class ShaderResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Shader* Get() const { return Ptr; }

private:
    Shader* Ptr = nullptr;
    
};

typedef Resource::Ref<ShaderResource> ResShader;  