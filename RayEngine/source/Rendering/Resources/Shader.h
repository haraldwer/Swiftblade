#pragma once

#include "Core/Resource/Resource.h"

// Raylib shader reference
struct Shader;

namespace Rendering
{
    class ShaderResource : Resource::Base
    {
        CLASS_INFO(ShaderResource, Resource::Base)
        
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

        bool Load() override;
        bool Unload() override;
        Utility::Timepoint GetEditTime() const override; 
        Shader* Get() const;

        int GetLocation(const String& InValue);
        int GetLocation(const DefaultLoc& InLoc) const;

    private:
        String LoadShaderFile(const String& InPath, Set<String>& InIncludes);

        static String ProcessIncludes(const String& InShaderCode, const String& InPath, Set<String>& InIncludes);
        String ProcessDefines(const String& InShaderCode);
        void LoadDefaultLocs();
        
        Shader* ptr = nullptr;
        Set<String> vsIncludes = {};
        Set<String> fsIncludes = {};
        Vector<String> defines = {};
        
        Map<String, int> locations = {};
        Array<int, static_cast<size_t>(DefaultLoc::COUNT)> defaultLocs = {};
    };
}

typedef Resource::Ref<Rendering::ShaderResource> ResShader;
