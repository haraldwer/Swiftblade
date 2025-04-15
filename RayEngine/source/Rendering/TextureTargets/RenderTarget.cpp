#include "RenderTarget.h"

#include "Rendering/Resources/Shader.h"
#include "raylib.h"
#include "rlgl.h"

bool RenderTarget::Setup(const RenderTexture& InTarget, const String& InName, uint8 InFormat)
{
    if (TryBeginSetup(InTarget))
    {
        CreateBuffer(InName, InFormat);
        EndSetup(InTarget);
        return true;
    }
    return false;
}

bool RenderTarget::TryBeginSetup(const RenderTexture& InRenderTexture)
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

void RenderTarget::EndSetup(const RenderTexture& InRenderTexture) const
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
            // Attach cubemap face later
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
    
    uint32 depthID = InRenderTexture.depth.id;
    CHECK_ASSERT(depthID == 0, "Invalid depth texture");
    rlFramebufferAttach(FrameBuffer, depthID, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    
    CHECK_ASSERT(!rlFramebufferComplete(FrameBuffer), "Framebuffer incomplete"); 
}

void RenderTarget::Unload()
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

void RenderTarget::BeginWrite(const int InBlend, const bool InClear, int InFaceIndex) const
{
    if (InFaceIndex >= 0)
    {
        // Set the cubemap face to target...
        for (const auto& tex : Textures)
        {
            CHECK_ASSERT(!tex.Tex, "Tex nullptr");
            rlFramebufferAttach(
                FrameBuffer,
                tex.Tex->id,
                RL_ATTACHMENT_COLOR_CHANNEL0,
                RL_ATTACHMENT_CUBEMAP_POSITIVE_X + InFaceIndex,
                0);
        }
    }
    
    rlEnableFramebuffer(FrameBuffer);
    
    if (InClear)
        rlClearScreenBuffers();
    if (InBlend >= RL_BLEND_ALPHA)
    {
        rlEnableColorBlend();
        rlSetBlendMode(InBlend);
    }
    else
    {
        rlDisableColorBlend();
    } 
}

void RenderTarget::EndWrite() const
{
    rlSetBlendMode(RL_BLEND_ALPHA);
    rlDisableColorBlend();
    rlDisableFramebuffer();
}

void RenderTarget::GenerateMips() const
{
    for (auto& tex : Textures)
        if (tex.Tex->mipmaps > 1)
            GenTextureMipmaps(tex.Tex);
}

void RenderTarget::Bind(ShaderResource& InShader, Slot& InOutSlots) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);
    for (const auto& tex : Textures)
    {
        InOutSlots.Index++;
        int loc = InShader.GetLocation(tex.Name);
        CHECK_CONTINUE(loc < 0 && !tex.Cubemap);

        InOutSlots.Loc++;
        CHECK_ASSERT(!tex.Tex, "Tex nullptr");
        
        rlTextureParameters(tex.Tex->id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
        rlTextureParameters(tex.Tex->id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
        
        rlActiveTextureSlot(InOutSlots.Loc);
        if (tex.Cubemap)
            rlEnableTextureCubemap(tex.Tex->id);
        else
            rlEnableTexture(tex.Tex->id);
        
        rlSetUniform(loc, &InOutSlots.Loc, SHADER_UNIFORM_INT, 1); // NOTE: Default texture is always activated as GL_TEXTURE0
        rlActiveTextureSlot(0);
    }
}

void RenderTarget::Unbind(ShaderResource& InShader, Slot& InOutSlots) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);
    for (const auto& buff : Textures)
    {
        InOutSlots.Index++;
        int loc = InShader.GetLocation(buff.Name);
        CHECK_CONTINUE(loc < 0);
        
        InOutSlots.Loc++;
        rlActiveTextureSlot(InOutSlots.Loc);
        rlDisableTexture();
        int l = 0;
        rlSetUniform(loc, &l, RL_SHADER_UNIFORM_SAMPLER2D, 1); // NOTE: Default texture is always activated as GL_TEXTURE0
        rlActiveTextureSlot(0);
    }
}

void RenderTarget::CreateBuffer(const String& InName, uint8 InPixelFormat, int InMips, bool InCubemap)
{
    int mips = InMips;
    if (mips <= 0)
        mips = 1 + static_cast<int>(floor(log(Utility::Math::Max(Width, Height)) / log(2)));
    
    auto& buffer = Textures.emplace_back();
    buffer.Name = InName;
    buffer.Cubemap = InCubemap;
    buffer.Tex = new Texture(); 
    buffer.Tex->id = InCubemap ?
        rlLoadTextureCubemap(nullptr, Width, InPixelFormat, mips) : 
        rlLoadTexture(nullptr, Width, Height, InPixelFormat, mips); 
    buffer.Tex->width = Width;
    buffer.Tex->height = InCubemap ? Width : Height;
    buffer.Tex->mipmaps = mips;
    buffer.Tex->format = InPixelFormat;
}

