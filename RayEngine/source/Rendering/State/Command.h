#pragma once

namespace Rendering
{
    struct TextureCommand
    {
        uint32 ID = static_cast<uint32>(-1); // Texture ID
        int ShaderLoc = -1; // Shader location
        int Wrap = -1;
        int Filter = -1;
        bool Cubemap = false;
    };
    
    struct ShaderCommand
    {
        uint32 ID = static_cast<uint32>(-1);
        int* Locs = nullptr;
        bool BackfaceCulling = true;
        bool DepthTest = false;
        bool DepthMask = false;
        int BlendMode = -1;
    };

    struct FrameCommand
    {
        uint32 fboID = static_cast<uint32>(-1);
        bool Clear = false;
    };

    struct PerspectiveCommand
    {
        Vec4I Rect;
    };

    struct MeshCommand
    {
        uint32 vaoID = static_cast<uint32>(-1);
    };
}