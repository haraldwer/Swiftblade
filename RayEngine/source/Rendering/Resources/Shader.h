#pragma once

#include "Core/Resource/Resource.h"

// Raylib shader reference
struct Shader;

class ShaderResource
{
public:

    // Common uniform ids that can be cached
    enum class DefaultLoc : uint8
    {
        TIME = 0,
        DELTA,
        RESOLUTION,
        VIEW_PROJ,
        VIEW_PROJ_PREV,
        VIEW_PROJ_INV,
        RECT,
        REF_RECT,
        CAMERA_POSITION,
        NEAR_FAR,
        DEFERRED_ID,
        COUNT
    };

    ShaderResource();
    
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const; 
    Shader* Get() const;

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName, uint32 InOffset = 0) { return false; };

    int GetLocation(const String& InValue);
    int GetLocation(const DefaultLoc& InLoc) const;

private:
    String LoadShaderFile(const String& InPath, Set<String>& InIncludes);

    static String ProcessIncludes(const String& InShaderCode, const String& InPath, Set<String>& InIncludes);
    String ProcessDefines(const String& InShaderCode);
    void LoadDefaultLocs();
    
    String identifier = {}; 
    Shader* ptr = nullptr;
    Set<String> vsIncludes = {};
    Set<String> fsIncludes = {};
    Vector<String> defines = {};
    
    Map<String, int> locations = {};
    Array<int, static_cast<size_t>(DefaultLoc::COUNT)> defaultLocs = {};
};

typedef Resource::Ref<ShaderResource> ResShader;  