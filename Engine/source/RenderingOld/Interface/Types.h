#pragma once

namespace Rendering
{
    enum class TextureType : uint8
    {
        TEXTURE,
        TEXTURE_3D,
        CUBEMAP,
    };
    
    enum class TextureFormat : uint8
    {
        RGB32,
        RGBA32,
        RGB16,
        RGBA16,
        RGB8,
        RGBA8,
        R32,
        R16
    };
    
    enum class TextureParam : uint8
    {
        MIN_FILTER,
        MAG_FILTER,
        WRAP_S,
        WRAP_T,
        WRAP_R,
    };
    
    enum class TextureParamValue : uint8
    {
        NONE,
        
        NEAREST,
        LINEAR,
        LINEAR_MIPMAP,
        
        CLAMP_TO_EDGE,
        WRAP,
        REPEAT,
        REPEAT_MIRROR,
    };
    
    enum class BlendMode : uint8
    {
        NONE,
        ALPHA,
        ADDITIVE,
        MULTIPLY,
        ADD_COLORS,
        SUBTRACT_COLORS,
        PREMULTIPLY,
    };
    
    enum class TextureAttachment : uint8
    {
        COLOR,
        DEPTH,
        STENCIL
    };
}