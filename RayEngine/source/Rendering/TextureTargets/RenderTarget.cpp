#include "RenderTarget.h"

#include "Rendering/Resources/Shader.h"
#include "raylib.h"
#include "rlgl.h"
#include "State/State.h"

bool Rendering::RenderTarget::Setup(const Vec3I& InRes, const String& InName, uint8 InFormat, int InDefaultFilter, TexType InType)
{
    if (TryBeginSetup(InRes))
    {
        CreateBuffer(InName, InFormat, 1.0, InDefaultFilter, InType);
        EndSetup();
        return true;
    }
    return false;
}

bool Rendering::RenderTarget::TryBeginSetup(const Vec3I& InRes)
{
    int targetWidth = Utility::Math::Max(InRes.x, 16);
    int targetHeight = Utility::Math::Max(InRes.y, 16);
    if (size.x == targetWidth &&
        size.y == targetHeight)
        return false;
    if (targetWidth <= 0 || targetHeight <= 0)
        return false;

    Unload(); 
    
    size = {
        targetWidth, 
        targetHeight, 
        InRes.z
    };

    // Create framebuffer
    frameBuffer = rlLoadFramebuffer();
    CHECK_ASSERT(!frameBuffer, "Unable to create framebuffer"); 
    rlEnableFramebuffer(frameBuffer);

    return true;
}

void Rendering::RenderTarget::EndSetup() const
{
    CHECK_RETURN(!frameBuffer);
    
    // Attach to framebuffer
    rlActiveDrawBuffers(static_cast<int>(textures.size())); // Count - Depth
    for (int i = 0; i < static_cast<int>(textures.size()); i++)
    {
        const auto& tex = textures[i];
        CHECK_ASSERT(!tex.tex, "Tex nullptr");
        rlFramebufferAttach(
            frameBuffer,
            tex.tex->id,
            RL_ATTACHMENT_COLOR_CHANNEL0 + i,
            RL_ATTACHMENT_TEXTURE2D,
            0);
    }
    
    CHECK_ASSERT(!rlFramebufferComplete(frameBuffer), "Framebuffer incomplete");
}

void Rendering::RenderTarget::AttachDepth(const RenderTexture& InTarget) const
{
    const uint32 depthID = InTarget.depth.id;
    CHECK_ASSERT(depthID == 0, "Invalid depth texture");
    rlFramebufferAttach(frameBuffer, depthID, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
}

void Rendering::RenderTarget::Unload()
{
    for (const TargetTex& buff : textures)
    {
        CHECK_ASSERT(!buff.tex, "Tex nullptr");
        rlUnloadTexture(buff.tex->id);
        delete (buff.tex);
    }
    textures.clear(); 

    if (frameBuffer)
    {
        rlFramebufferAttach(frameBuffer, 0, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
        rlUnloadFramebuffer(frameBuffer);
        frameBuffer = 0;
    }
    size = {};
}

void Rendering::RenderTarget::AttachAsTarget(int InLayerFace)
{
    int i = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    for (const TargetTex& buff : textures)
    {
        // These types require additional attachment logic
        switch (buff.type) {
            case TexType::TEX_3D:
                glFramebufferTextureLayer(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0 + i,
                    buff.tex->id,
                    0,
                    InLayerFace
                );
                break;
            case TexType::CUBEMAP:
                glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
                GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + InLayerFace;
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0 + i,
                    face,
                    buff.tex->id,
                    0
                );
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                break;
        }
        i++;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Rendering::RenderTarget::Bind(ShaderResource& InShader, int& InOutSlot, const int InFilter, const String& InPostfix) const
{
    for (const auto& tex : textures)
    {
        const int loc = InShader.GetLocation(tex.name + InPostfix);
        CHECK_CONTINUE(loc < 0);
        CHECK_ASSERT(!tex.tex, "Tex nullptr");

        InOutSlot++;
        TextureCommand cmd;
        cmd.shaderLoc = loc;
        cmd.id = tex.tex->id;
        cmd.filter = InFilter < 0 ? tex.defaultFilter : InFilter;
        rlState::current.Set(cmd, InOutSlot);

        const int scaleLoc = InShader.GetLocation(tex.name + InPostfix + "Scale");
        if (scaleLoc >= 0)
        {
            Vec2F size = tex.scaledSize.xy.To<float>();
            Vec2F ref = Vec2I(tex.tex->width, tex.tex->height).To<float>();
            Vec2F scale = size / ref;
            rlSetUniform(scaleLoc, &scale, RL_SHADER_UNIFORM_VEC2, 1);
        }
        
        const int texelLoc = InShader.GetLocation(tex.name + InPostfix + "Texel");
        if (texelLoc >= 0)
        {
            Vec2F texel = Vec2F(1.0f) / tex.scaledSize.xy.To<float>();
            rlSetUniform(texelLoc, &texel, RL_SHADER_UNIFORM_VEC2, 1);
        }

        const int sizeLoc = InShader.GetLocation(tex.name + InPostfix + "Size");
        if (sizeLoc >= 0)
        {
            Vec2F size = tex.scaledSize.xy.To<float>();
            rlSetUniform(sizeLoc, &size, RL_SHADER_UNIFORM_VEC2, 1);
        }
    }
}

int rlLoadTexture3D(int width, int height, int depth, int format)
{
    unsigned int glInternalFormat, glFormat, glType;
    rlGetGlTextureFormats(format, &glInternalFormat, &glFormat, &glType);
    
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_3D, texID);

    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        glInternalFormat,          // internal format
        width,
        height,
        depth,
        0,
        glFormat,
        glType,
        NULL                // no initial data
    );

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_3D, 0);

    return static_cast<int>(texID);
}

void Rendering::RenderTarget::CreateBuffer(const String &InName, uint8 InPixelFormat, float InResScale, int InDefaultFilter, int InMips, TexType InType)
{
    const float scale = Utility::Math::Max(1.0f, InResScale);
    const auto scaled = (size.To<float>() * scale).To<int>(); 
    CHECK_RETURN_LOG(scaled.x <= 0 || scaled.y <= 0, "Buffer too small");
    CHECK_RETURN_LOG(scaled.z <= 0 && InType == TexType::TEX_3D, "Buffer too small");
    
    int mips = InMips;
    if (mips <= 0)
        mips = 1 + static_cast<int>(floor(log(Utility::Math::Max(scaled.x, scaled.y)) / log(2)));
    if (InType == TexType::TEX_3D)
        mips = 0;

#ifdef GRAPHICS_API_OPENGL_ES3
    // Fake a smaller resolution target (cannot have resolution mismatch)
    const Vec3I texSize = size;
#else
    const Vec3I texSize = scaled;
#endif
    
    auto& buffer = textures.emplace_back();
    buffer.name = InName;
    buffer.type = InType;
    buffer.defaultFilter = InDefaultFilter;
    buffer.scaledSize = scaled;
    buffer.tex = new Texture(); 
    
    switch (InType)
    {
        case TexType::TEX:
            buffer.tex->id = rlLoadTexture(nullptr, texSize.x, texSize.y, InPixelFormat, mips);
            break;
        case TexType::TEX_3D:
            buffer.tex->id = rlLoadTexture3D(texSize.x, texSize.y, texSize.z, InPixelFormat);
            break;
        case TexType::CUBEMAP:
            buffer.tex->id = rlLoadTextureCubemap(nullptr, texSize.x, InPixelFormat, mips);
            break;
    }
    
    buffer.tex->width = texSize.x;
    buffer.tex->height = texSize.y;
    buffer.tex->mipmaps = mips;
    buffer.tex->format = InPixelFormat;
}
