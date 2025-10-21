#pragma once

namespace Rendering
{
    struct UniformCommand
    {
        uint32 valueHash = 0;
        int loc = -1;
        const void* ptr = nullptr;
        int type = 0;
        int count = 0;
        bool mat = false;

        bool operator==(const UniformCommand& InOther) const
        {
            return
                InOther.valueHash == valueHash && 
                InOther.loc == loc && 
                InOther.ptr == ptr && 
                InOther.type == type && 
                InOther.count == count && 
                InOther.mat == mat; 
        }
    };
    
    struct TextureCommand
    {
        uint32 id = static_cast<uint32>(-1); // Texture ID
        int shaderLoc = -1; // Shader location
        int wrap = -1;
        int filter = -1;
        bool cubemap = false;

        bool operator==(const TextureCommand& InOther) const
        {
            return
                InOther.id == id &&
                InOther.shaderLoc == shaderLoc &&
                InOther.wrap == wrap &&
                InOther.filter == filter &&
                InOther.cubemap == cubemap;
        }
    };
    
    struct ShaderCommand
    {
        uint32 id = static_cast<uint32>(-1);
        int* locs = nullptr;
        bool backfaceCulling = true;
        bool depthTest = false;
        bool depthMask = false;
        int blendMode = -1;
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
    };

    struct MeshCommand
    {
        uint32 vaoID = static_cast<uint32>(-1);
    };
}
