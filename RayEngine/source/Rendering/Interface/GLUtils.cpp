#include "GLUtils.h"

#include "external/glad.h"

int Rendering::GL::GetType(const TextureType InType)
{
    switch (InType)
    {
        case TextureType::TEXTURE: return GL_TEXTURE_2D;
        case TextureType::TEXTURE_3D: return GL_TEXTURE_3D;
        case TextureType::CUBEMAP: return GL_TEXTURE_CUBE_MAP;
        default: break;
    }
    CHECK_ASSERT(false, "Invalid type");
    return 0;
}

int Rendering::GL::GetFormat(const TextureFormat InFormat)
{
    switch (InFormat)
    {
        case TextureFormat::RGB32: return GL_RGB32F;
        case TextureFormat::RGBA32: return GL_RGBA32F;
        case TextureFormat::RGB16: return GL_RGB16;
        case TextureFormat::RGBA16: return GL_RGBA16;
        case TextureFormat::RGB8: return GL_RGB8;
        case TextureFormat::RGBA8: return GL_RGBA8;
        case TextureFormat::R32: return GL_R32F;
        default: break;
    }
    CHECK_ASSERT(false, "Invalid format");
    return 0;
}

void Rendering::GL::GetInternalFormats(TextureFormat InFormat, unsigned int *glInternalFormat, unsigned int *glFormat, unsigned int *glType)
{
    *glInternalFormat = 0;
    *glFormat = 0;
    *glType = 0;

    switch (InFormat)
    {
        case TextureFormat::RGB32: *glInternalFormat = GL_RGB32F; *glFormat = GL_RGB; *glType = GL_FLOAT; break;
        case TextureFormat::RGBA32: *glInternalFormat = GL_RGBA32F; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;
        case TextureFormat::RGB16: *glInternalFormat = GL_RGB16F; *glFormat = GL_RGB; *glType = GL_HALF_FLOAT; break;
        case TextureFormat::RGBA16: *glInternalFormat = GL_RGBA16F; *glFormat = GL_RGBA; *glType = GL_HALF_FLOAT; break;
        case TextureFormat::RGB8: *glInternalFormat = GL_RGB8; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case TextureFormat::RGBA8: *glInternalFormat = GL_RGBA8; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        case TextureFormat::R32: *glInternalFormat = GL_R32F; *glFormat = GL_RED; *glType = GL_FLOAT; break;
        case TextureFormat::R16: *glInternalFormat = GL_R16F; *glFormat = GL_RED; *glType = GL_HALF_FLOAT; break;
        default: CHECK_ASSERT(false, "Unknown format") break;
    }
    
    /*
#ifdef GRAPHICS_API_OPENGL_ES3
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: *glInternalFormat = GL_LUMINANCE; *glFormat = GL_LUMINANCE; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: *glInternalFormat = GL_LUMINANCE_ALPHA; *glFormat = GL_LUMINANCE_ALPHA; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: *glInternalFormat = GL_RGB; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_R32F_EXT; *glFormat = GL_RED_EXT; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGB32F_EXT; *glFormat = GL_RGB; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGBA32F_EXT; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R16: if (RLGL.ExtSupported.texFloat16) *glInternalFormat = GL_R16F_EXT; *glFormat = GL_RED_EXT; *glType = GL_HALF_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16: if (RLGL.ExtSupported.texFloat16) *glInternalFormat = GL_RGB16F_EXT; *glFormat = GL_RGB; *glType = GL_HALF_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: if (RLGL.ExtSupported.texFloat16) *glInternalFormat = GL_RGBA16F_EXT; *glFormat = GL_RGBA; *glType = GL_HALF_FLOAT; break;
#else
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: *glInternalFormat = GL_R8; *glFormat = GL_RED; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: *glInternalFormat = GL_RG8; *glFormat = GL_RG; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5: *glInternalFormat = GL_RGB565; *glFormat = GL_RGB; *glType = GL_UNSIGNED_SHORT_5_6_5; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: *glInternalFormat = GL_RGB8; *glFormat = GL_RGB; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: *glInternalFormat = GL_RGB5_A1; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: *glInternalFormat = GL_RGBA4; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: *glInternalFormat = GL_RGBA8; *glFormat = GL_RGBA; *glType = GL_UNSIGNED_BYTE; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_R32F; *glFormat = GL_RED; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGB32F; *glFormat = GL_RGB; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: if (RLGL.ExtSupported.texFloat32) *glInternalFormat = GL_RGBA32F; *glFormat = GL_RGBA; *glType = GL_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R16: if (RLGL.ExtSupported.texFloat16) *glInternalFormat = GL_R16F; *glFormat = GL_RED; *glType = GL_HALF_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16: if (RLGL.ExtSupported.texFloat16) *glInternalFormat = GL_RGB16F; *glFormat = GL_RGB; *glType = GL_HALF_FLOAT; break;
        case RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: if (RLGL.ExtSupported.texFloat16) *glInternalFormat = GL_RGBA16F; *glFormat = GL_RGBA; *glType = GL_HALF_FLOAT; break;
#endif
    */
}

int Rendering::GL::GetParam(const TextureParam InParam)
{
    switch (InParam)
    {
        case TextureParam::MIN_FILTER: return GL_TEXTURE_MIN_FILTER;
        case TextureParam::MAG_FILTER: return GL_TEXTURE_MAG_FILTER;
        case TextureParam::WRAP_S: return GL_TEXTURE_WRAP_S;
        case TextureParam::WRAP_T: return GL_TEXTURE_WRAP_T;
        case TextureParam::WRAP_R: return GL_TEXTURE_WRAP_R;
        default:break;
    }
    CHECK_ASSERT(false, "Invalid param");
    return 0;
}

int Rendering::GL::GetParamValue(const TextureParamValue InValue)
{
    switch (InValue)
    {
        case TextureParamValue::NEAREST: return GL_NEAREST;
        case TextureParamValue::LINEAR: return GL_LINEAR;
        case TextureParamValue::LINEAR_MIPMAP: return GL_LINEAR_MIPMAP_LINEAR;
        case TextureParamValue::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case TextureParamValue::WRAP: return GL_INCR_WRAP;
        case TextureParamValue::REPEAT: return GL_REPEAT;
        case TextureParamValue::REPEAT_MIRROR: return GL_MIRRORED_REPEAT;
        default:break;
    }
    CHECK_ASSERT(false, "Invalid param");
    return 0;
}

int Rendering::GL::GetAttachment(const TextureAttachment InAttachment)
{
    switch (InAttachment) {
        case TextureAttachment::COLOR: return GL_COLOR_ATTACHMENT0;
        case TextureAttachment::DEPTH: return GL_DEPTH_ATTACHMENT;
        case TextureAttachment::STENCIL: return GL_STENCIL_ATTACHMENT;
        default:break;
    }
    CHECK_ASSERT(false, "Invalid param");
    return 0;
}
