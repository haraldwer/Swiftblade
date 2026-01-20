#include "Textures.h"

#include "GLUtils.h"

uint32 Rendering::RHI::CreateFramebuffer()
{
    uint32 fboId = 0;
    glGenFramebuffers(1, &fboId);
    return fboId;
}

void Rendering::RHI::DestroyFramebuffer(uint32 InBuffer)
{
    glDeleteFramebuffers(1, &InBuffer);
}

void Rendering::RHI::EnableFramebuffer(const uint32 InBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, InBuffer);
}

void Rendering::RHI::DisableFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Rendering::RHI::FramebufferComplete()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    CHECK_RETURN(status == GL_FRAMEBUFFER_COMPLETE, true);
    CHECK_ASSERT(status == GL_FRAMEBUFFER_UNSUPPORTED, "Framebuffer is unsupported");
    CHECK_ASSERT(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, "Framebuffer has incomplete attachment");
    CHECK_ASSERT(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, "Framebuffer has a missing attachment");
#if defined(GRAPHICS_API_OPENGL_ES2)
    CHECK_ASSERT(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS, "Framebuffer has incomplete dimensions");
#endif
    CHECK_ASSERT(false, "Unknown framebuffer error");
    return false;
}

void Rendering::RHI::ActivateDrawBuffers(int InCount)
{
    CHECK_ASSERT(InCount > 8, "GL: Max color buffers limited to 8");
    if (InCount <= 0)
        InCount = 1;
    
    unsigned int buffers[8] = {
#if defined(GRAPHICS_API_OPENGL_ES3)
        GL_COLOR_ATTACHMENT0_EXT,
        GL_COLOR_ATTACHMENT1_EXT,
        GL_COLOR_ATTACHMENT2_EXT,
        GL_COLOR_ATTACHMENT3_EXT,
        GL_COLOR_ATTACHMENT4_EXT,
        GL_COLOR_ATTACHMENT5_EXT,
        GL_COLOR_ATTACHMENT6_EXT,
        GL_COLOR_ATTACHMENT7_EXT,
#else
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
#endif
    };

#if defined(GRAPHICS_API_OPENGL_ES3)
    glDrawBuffersEXT(InCount, buffers);
#else
    glDrawBuffers(InCount, buffers);
#endif
}

void Rendering::RHI::FramebufferAttach(uint32 InTexture, TextureType InType, TextureAttachment InAttachment, const int InSlot, const int InMip, const int InSide)
{
    int target = GL_FRAMEBUFFER;
    int attachment = GL::GetAttachment(InAttachment);
    if (InAttachment == TextureAttachment::COLOR)
    {
        CHECK_ASSERT(InSlot > 8, "Only 8 color slots");
        attachment += InSlot;
    }
    
    switch (InType)
    {
        case TextureType::TEXTURE: glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, InTexture, InMip); break;
        case TextureType::CUBEMAP: glFramebufferTexture2D(target, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + InSide, InTexture, InMip); break;
        case TextureType::TEXTURE_3D: glFramebufferTexture3D(target, attachment, GL_TEXTURE_3D, InTexture, InMip, InSide); break;
    }
}

uint32 Rendering::RHI::CreateTexture(const Vec3I &InRes, TextureFormat InFormat, const TextureType InType, int InMips)
{
    CHECK_ASSERT(InType == TextureType::TEXTURE_3D && InMips > 1, "3D textures do not support mips");
    CHECK_ASSERT(InType == TextureType::TEXTURE && InRes.z > 1, "Textures does not support layers");
    
    int type = GL::GetType(InType);
    unsigned int glInternalFormat, glFormat, glType;
    GL::GetInternalFormats(InFormat, &glInternalFormat, &glFormat, &glType);
        
    GLuint texID;
    glGenTextures(1, &texID);
    BindTexture(texID, InType);
    
    if (InMips < 1) 
        InMips = 1;
    int maxMip = static_cast<int>(floor(log2(InRes.Min()))) + 1;
    InMips = Utility::Math::Min(InMips, maxMip);
    
    switch (InType)
    {
        case TextureType::TEXTURE:
            for (int mip = 0; mip < InMips; mip++)
            {
                int mipSize = Utility::Math::Max(1, InRes.Max() >> mip);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    mip,
                    static_cast<GLint>(glInternalFormat),
                    mipSize,
                    mipSize,
                    0,
                    glFormat,
                    glType,
                    nullptr
                );
            }
            break;
            
        case TextureType::TEXTURE_3D:
            glTexImage3D(
                GL_TEXTURE_3D,
                0,
                glInternalFormat,
                InRes.x,
                InRes.y,
                InRes.z,
                0,
                glFormat,
                glType,
                nullptr
            );
            break;
            
        case TextureType::CUBEMAP:
            for (int mip = 0; mip < InMips; mip++)
            {
                int mipSize = Utility::Math::Max(1, InRes.Max() >> mip);
                for (int face = 0; face < 6; face++)
                {
                    glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                        mip,
                        static_cast<GLint>(glInternalFormat),
                        mipSize,
                        mipSize,
                        0,
                        glFormat,
                        glType,
                        nullptr
                    );
                }
            
            }
            break;
    }
    
    if (InMips > 1)
    {
        glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, InMips - 1);
    }
    
    BindTexture(0, InType);

    return texID;
}

uint32 Rendering::RHI::CreateTextureView(const uint32 InTex, const TextureFormat InFormat, const int InLayer) 
{
    uint32 id = 0;
    glGenTextures(1, &id);
    CHECK_ASSERT(id == 0, "Failed to create texture");
    unsigned int glInternalFormat, glFormat, glType;
    GL::GetInternalFormats(InFormat, &glInternalFormat, &glFormat, &glType);
    glTextureView(
        id,
        GL_TEXTURE_2D,
        InTex,
        glInternalFormat,
        0,
        1,
        InLayer, 
        1
    );
    return id;
}

uint32 Rendering::RHI::CreateTextureDepth(const Vec2I& InSize, const TextureType InType) 
{
    CHECK_ASSERT(InType == TextureType::TEXTURE_3D, "3D depth not supported");
    uint32 id = 0;
    constexpr uint32 format = GL_DEPTH_COMPONENT32;
    const int type = GL::GetType(InType);
        
    glGenTextures(1, &id);
    BindTexture(id, InType);
    glTexImage2D(
        type, 
        0, 
        format, 
        InSize.x, 
        InSize.y, 
        0, 
        GL_DEPTH_COMPONENT, 
        GL_UNSIGNED_INT, 
        nullptr);
    
    SetTextureParam(TextureParam::MIN_FILTER, TextureParamValue::NEAREST, InType);
    SetTextureParam(TextureParam::MAG_FILTER, TextureParamValue::NEAREST, InType);
    SetTextureParam(TextureParam::WRAP_S, TextureParamValue::CLAMP_TO_EDGE, InType);
    SetTextureParam(TextureParam::WRAP_T, TextureParamValue::CLAMP_TO_EDGE, InType);

    glBindTexture(type, 0);

    return id;
}

void Rendering::RHI::DestroyTexture(uint32 InTexture)
{
    glDeleteTextures(1, &InTexture);
}

void Rendering::RHI::ActivateTextureSlot(const int InSlot)
{
    glActiveTexture(GL_TEXTURE0 + InSlot);
}

void Rendering::RHI::EnableTexture(const unsigned int id, const TextureType InType) 
{
    glBindTexture(GL::GetType(InType), id);
}

void Rendering::RHI::DisableTexture(const TextureType InType) 
{
    glBindTexture(GL::GetType(InType), 0);
}

void Rendering::RHI::BindTexture(const uint32 InID, const TextureType InType) 
{
    glBindTexture(GL::GetType(InType), InID);
}

void Rendering::RHI::UnbindTexture(const TextureType InType) 
{
    glBindTexture(GL::GetType(InType), 0);
}

void Rendering::RHI::SetBlendMode(const BlendMode InMode) 
{
    switch (InMode)
    {
        case BlendMode::ALPHA:              glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  glBlendEquation(GL_FUNC_ADD); break;
        case BlendMode::ADDITIVE:           glBlendFunc(GL_SRC_ALPHA, GL_ONE);                  glBlendEquation(GL_FUNC_ADD); break;
        case BlendMode::MULTIPLY:           glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);  glBlendEquation(GL_FUNC_ADD); break;
        case BlendMode::ADD_COLORS:         glBlendFunc(GL_ONE, GL_ONE);                        glBlendEquation(GL_FUNC_ADD); break;
        case BlendMode::SUBTRACT_COLORS:    glBlendFunc(GL_ONE, GL_ONE);                        glBlendEquation(GL_FUNC_SUBTRACT); break;
        case BlendMode::PREMULTIPLY:        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);        glBlendEquation(GL_FUNC_ADD); break;
        
        case BlendMode::NONE:
        default:
            glDisable(GL_BLEND);
            return;
    }
    glEnable(GL_BLEND);
}

void Rendering::RHI::SetTextureParam(const TextureParam InParam, const TextureParamValue InValue, const TextureType InType) 
{
    int type = GL::GetType(InType);
    int param = GL::GetParam(InParam);
    int value = GL::GetParamValue(InValue);
    glTexParameteri(type, param, value);
}
