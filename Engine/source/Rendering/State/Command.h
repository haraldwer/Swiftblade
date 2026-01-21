#pragma once
#include "Interface/Shaders.h"
#include "Interface/Types.h"

namespace Rendering
{
    struct UniformCommand
    {
        uint32 valueHash = 0;
        int loc = -1;
        const void* ptr = nullptr;
        UniformType type = UniformType::FLOAT;
        int count = 0;

        bool operator==(const UniformCommand& InOther) const
        {
            if (InOther.valueHash == 0 || valueHash == 0)
                return false;
            return
                InOther.valueHash == valueHash && 
                InOther.loc == loc && 
                InOther.type == type && 
                InOther.count == count;
        }
    };
    
    struct TextureCommand
    {
        uint32 id = static_cast<uint32>(-1); // Texture ID
        int shaderLoc = -1; // Shader location
        TextureParamValue wrap = TextureParamValue::NONE;
        TextureParamValue filter = TextureParamValue::NONE;
        TextureType type = TextureType::TEXTURE;

        bool operator==(const TextureCommand& InOther) const
        {
            return
                InOther.id == id &&
                InOther.shaderLoc == shaderLoc &&
                InOther.wrap == wrap &&
                InOther.filter == filter &&
                InOther.type == type;
        }
    };
    
    struct ShaderCommand
    {
        uint32 id = static_cast<uint32>(-1);
        int* locs = nullptr;
        bool backfaceCulling = true;
        bool depthTest = false;
        bool depthMask = false;
        BlendMode blendMode = BlendMode::NONE;
    };

    struct FrameCommand
    {
        uint32 fboID = static_cast<uint32>(-1);
        Vec2I size = {};
        bool clearTarget = false;
        bool clearDepth = false;
    };

    struct PerspectiveCommand
    {
        Vec4F rect = {};
        int layer = -1;
    };

    struct MeshCommand
    {
        uint32 vaoID = static_cast<uint32>(-1);
    };
}
