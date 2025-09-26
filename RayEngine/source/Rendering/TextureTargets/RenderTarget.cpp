#include "RenderTarget.h"

#include "Rendering/Resources/Shader.h"
#include "raylib.h"
#include "rlgl.h"
#include "State/State.h"

bool Rendering::RenderTarget::Setup(const RenderTexture& InTarget, const String& InName, const uint8 InFormat, float InResScale, int InDefaultFilter)
{
    if (TryBeginSetup(InTarget, InResScale))
    {
        CreateBuffer(InName, InFormat, 1.0, InDefaultFilter);
        EndSetup(InTarget);
        return true;
    }
    return false;
}

bool Rendering::RenderTarget::TryBeginSetup(const RenderTexture& InRenderTexture, float InResScale)
{
    int targetWidth = Utility::Math::Max(InRenderTexture.texture.width * InResScale, 1.0f);
    int targetHeight = Utility::Math::Max(InRenderTexture.texture.height * InResScale, 1.0f);
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

void Rendering::RenderTarget::EndSetup(const RenderTexture& InRenderTexture) const
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

    const uint32 depthID = InRenderTexture.depth.id;
    CHECK_ASSERT(depthID == 0, "Invalid depth texture");
    rlFramebufferAttach(frameBuffer, depthID, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

#ifndef __EMSCRIPTEN__
    CHECK_ASSERT(!rlFramebufferComplete(frameBuffer), "Framebuffer incomplete");
#endif
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
        cmd.filter = InFilter == -1 ? tex.defaultFilter : InFilter;
        rlState::current.Set(cmd, InOutSlot);
    }
}

void Rendering::RenderTarget::CreateBuffer(const String& InName, const uint8 InPixelFormat, const float InResScale, int InDefaultFilter, const int InMips, const bool InCubemap)
{
    const int w = static_cast<int>(static_cast<float>(width) * InResScale);
    const int h = static_cast<int>(static_cast<float>(height) * InResScale);
    CHECK_RETURN_LOG(w <= 0 || h <= 0, "Buffer too small");
    
    int mips = InMips;
    if (mips <= 0)
        mips = 1 + static_cast<int>(floor(log(Utility::Math::Max(w, h)) / log(2)));
    
    auto& buffer = textures.emplace_back();
    buffer.name = InName;
    buffer.cubemap = InCubemap;
    buffer.defaultFilter = InDefaultFilter;
    buffer.tex = new Texture(); 
    buffer.tex->id = InCubemap ?
        rlLoadTextureCubemap(nullptr, w, InPixelFormat, mips) : 
        rlLoadTexture(nullptr, w, h, InPixelFormat, mips); 
    buffer.tex->width = w;
    buffer.tex->height = InCubemap ? w : h;
    buffer.tex->mipmaps = mips;
    buffer.tex->format = InPixelFormat;
}

