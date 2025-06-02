#pragma once

namespace Rendering
{
    struct UniformCommand
    {
        uint32 valueHash;
    };
    
    struct TextureCommand
    {
        uint32 id = static_cast<uint32>(-1); // Texture ID
        int shaderLoc = -1; // Shader location
        int wrap = -1;
        int filter = -1;
        bool cubemap = false;
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
        Vec4I rect = {};
    };

    struct MeshCommand
    {
        uint32 vaoID = static_cast<uint32>(-1);
    };
}