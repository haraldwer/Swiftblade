#include "RenderTarget.h"

#include "Rendering/Resources/Shader.h"
#include "raylib.h"
#include "rlgl.h"
#include "State/State.h"

bool Rendering::RenderTarget::Setup(const Vec2I& InRes, const String& InName, uint8 InFormat, int InDefaultFilter)
{
    if (TryBeginSetup(InRes))
    {
        CreateBuffer(InName, InFormat, 1.0, InDefaultFilter);
        EndSetup();
        return true;
    }
    return false;
}

bool Rendering::RenderTarget::TryBeginSetup(const Vec2I& InRes)
{
    int targetWidth = Utility::Math::Max(InRes.x, 16);
    int targetHeight = Utility::Math::Max(InRes.y, 16);
    if (width == targetWidth &&
        height == targetHeight)
        return false;
    if (targetWidth <= 0 || targetHeight <= 0)
        return false;

    Unload(); 
    
    width = targetWidth;
    height = targetHeight;

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

        if (tex.cubemap)
        {
            for (int face = 0; face < 6; face++)
            {
                CHECK_ASSERT(!tex.tex, "Tex nullptr");
                rlFramebufferAttach(
                    frameBuffer,
                    tex.tex->id,
                    RL_ATTACHMENT_COLOR_CHANNEL0 + i,
                    RL_ATTACHMENT_CUBEMAP_POSITIVE_X + face,
                    0);
            }
        }
        else
        {
            rlFramebufferAttach(
                frameBuffer,
                tex.tex->id,
                RL_ATTACHMENT_COLOR_CHANNEL0 + i,
                RL_ATTACHMENT_TEXTURE2D,
                0);
        }
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
    width = 0;
    height = 0; 
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
            Vec2F size = Vec2I(tex.scaledWidth, tex.scaledHeight).To<float>();
            Vec2F ref = Vec2I(tex.tex->width, tex.tex->height).To<float>();
            Vec2F scale = size / ref;
            rlSetUniform(scaleLoc, &scale, RL_SHADER_UNIFORM_VEC2, 1);
        }
        
        const int texelLoc = InShader.GetLocation(tex.name + InPostfix + "Texel");
        if (texelLoc >= 0)
        {
            Vec2F texel = Vec2F(1.0f) / Vec2I(tex.scaledWidth, tex.scaledHeight).To<float>();
            rlSetUniform(texelLoc, &texel, RL_SHADER_UNIFORM_VEC2, 1);
        }

        const int sizeLoc = InShader.GetLocation(tex.name + InPostfix + "Size");
        if (sizeLoc >= 0)
        {
            Vec2F size = Vec2I(tex.scaledWidth, tex.scaledHeight).To<float>();
            rlSetUniform(sizeLoc, &size, RL_SHADER_UNIFORM_VEC2, 1);
        }
    }
}

void Rendering::RenderTarget::CreateBuffer(const String& InName, const uint8 InPixelFormat, const float InResScale, int InDefaultFilter, const int InMips, const bool InCubemap)
{
    const float scale = Utility::Math::Max(1.0f, InResScale);
    const int w = static_cast<int>(static_cast<float>(width) * scale);
    const int h = static_cast<int>(static_cast<float>(height) * scale);
    CHECK_RETURN_LOG(w <= 0 || h <= 0, "Buffer too small");
    
    int mips = InMips;
    if (mips <= 0)
        mips = 1 + static_cast<int>(floor(log(Utility::Math::Max(w, h)) / log(2)));

#ifdef GRAPHICS_API_OPENGL_ES3
    // Fake a smaller resolution target (cannot have resolution mismatch)
    const int texWidth = width;
    const int texHeight = height;
#else
    const int texWidth = w;
    const int texHeight = h;
#endif
    
    auto& buffer = textures.emplace_back();
    buffer.name = InName;
    buffer.cubemap = InCubemap;
    buffer.defaultFilter = InDefaultFilter;
    buffer.scaledWidth = w;
    buffer.scaledHeight = h;
    buffer.tex = new Texture(); 
    buffer.tex->id = InCubemap ?
        rlLoadTextureCubemap(nullptr, texWidth, InPixelFormat, mips) : 
        rlLoadTexture(nullptr, texWidth, texHeight, InPixelFormat, mips);
    buffer.tex->width = texWidth;
    buffer.tex->height = InCubemap ? texWidth : texHeight;
    buffer.tex->mipmaps = mips;
    buffer.tex->format = InPixelFormat;
}
