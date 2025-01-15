#pragma once

#include "Resource/Resource.h"

// Raylib shader reference
struct Shader;

class ShaderResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Shader* Get() const;

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName) { return false; };

    int GetLocation(const String& InValue);

private:
    String LoadShaderFile(const String& InPath, Set<String>& InIncludes);
    String ProcessIncludes(const String& InShaderCode, const String& InPath, Set<String>& InIncludes);
    
    String Identifier; 
    Shader* Ptr = nullptr;
    Set<String> VSIncludes;
    Set<String> FSIncludes;
    Map<String, int> Locations;
};

typedef Resource::Ref<ShaderResource> ResShader;  