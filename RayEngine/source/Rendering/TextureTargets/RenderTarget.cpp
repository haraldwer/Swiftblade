#include "RenderTarget.h"

#include "Rendering/Resources/Shader.h"
#include "raylib.h"
#include "rlgl.h"
#include "State/State.h"

bool Rendering::RenderTarget::Setup(const RenderTexture& InTarget, const String& InName, const uint8 InFormat)
{
    if (TryBeginSetup(InTarget))
    {
        CreateBuffer(InName, InFormat);
        EndSetup(InTarget);
        return true;
    }
    return false;
}

bool Rendering::RenderTarget::TryBeginSetup(const RenderTexture& InRenderTexture)
{
    if (Width == InRenderTexture.texture.width &&
        Height == InRenderTexture.texture.height)
        return false;

    Unload(); 
    
    Width = InRenderTexture.texture.width;
    Height = InRenderTexture.texture.height;

    // Create framebuffer
    FrameBuffer = rlLoadFramebuffer();
    CHECK_ASSERT(!FrameBuffer, "Unable to create framebuffer"); 
    rlEnableFramebuffer(FrameBuffer);

    return true;
}

void Rendering::RenderTarget::EndSetup(const RenderTexture& InRenderTexture) const
{
    CHECK_RETURN(!FrameBuffer);
    
    // Attach to framebuffer
    rlActiveDrawBuffers(static_cast<int>(Textures.size())); // Count - Depth
    for (int i = 0; i < static_cast<int>(Textures.size()); i++)
    {
        const auto& tex = Textures[i];
        CHECK_ASSERT(!tex.Tex, "Tex nullptr");

        if (tex.Cubemap)
        {
            for (int face = 0; face < 6; face++)
            {
                CHECK_ASSERT(!tex.Tex, "Tex nullptr");
                rlFramebufferAttach(
                    FrameBuffer,
                    tex.Tex->id,
                    RL_ATTACHMENT_COLOR_CHANNEL0 + i,
                    RL_ATTACHMENT_CUBEMAP_POSITIVE_X + face,
                    0);
            }
        }
        else
        {
            rlFramebufferAttach(
                FrameBuffer,
                tex.Tex->id,
                RL_ATTACHMENT_COLOR_CHANNEL0 + i,
                RL_ATTACHMENT_TEXTURE2D,
                0);
        }
    }

    const uint32 depthID = InRenderTexture.depth.id;
    CHECK_ASSERT(depthID == 0, "Invalid depth texture");
    rlFramebufferAttach(FrameBuffer, depthID, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    
    CHECK_ASSERT(!rlFramebufferComplete(FrameBuffer), "Framebuffer incomplete"); 
}

void Rendering::RenderTarget::Unload()
{
    for (const TargetTex& buff : Textures)
    {
        CHECK_ASSERT(!buff.Tex, "Tex nullptr");
        rlUnloadTexture(buff.Tex->id);
        delete (buff.Tex);
    }
    Textures.clear(); 

    if (FrameBuffer)
    {
        rlFramebufferAttach(FrameBuffer, 0, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
        rlUnloadFramebuffer(FrameBuffer);
        FrameBuffer = 0;
    }
    Width = 0;
    Height = 0; 
}

void Rendering::RenderTarget::Bind(ShaderResource& InShader, int& InOutSlot, const int InFilter, const String& InPostfix) const
{
    for (const auto& tex : Textures)
    {
        const int loc = InShader.GetLocation(tex.Name + InPostfix);
        CHECK_CONTINUE(loc < 0);
        CHECK_ASSERT(!tex.Tex, "Tex nullptr");

        InOutSlot++;
        TextureCommand cmd;
        cmd.shaderLoc = loc;
        cmd.id = tex.Tex->id;
        cmd.filter = InFilter;
        rlState::current.Set(cmd, InOutSlot);
    }
}

void Rendering::RenderTarget::CreateBuffer(const String& InName, const uint8 InPixelFormat, const float InResScale, const int InMips, const bool InCubemap)
{
    const int w = static_cast<int>(static_cast<float>(Width) * InResScale);
    const int h = static_cast<int>(static_cast<float>(Height) * InResScale);
    
    int mips = InMips;
    if (mips <= 0)
        mips = 1 + static_cast<int>(floor(log(Utility::Math::Max(w, h)) / log(2)));
    
    auto& buffer = Textures.emplace_back();
    buffer.Name = InName;
    buffer.Cubemap = InCubemap;
    buffer.Tex = new Texture(); 
    buffer.Tex->id = InCubemap ?
        rlLoadTextureCubemap(nullptr, w, InPixelFormat, mips) : 
        rlLoadTexture(nullptr, w, h, InPixelFormat, mips); 
    buffer.Tex->width = w;
    buffer.Tex->height = InCubemap ? w : h;
    buffer.Tex->mipmaps = mips;
    buffer.Tex->format = InPixelFormat;
}

