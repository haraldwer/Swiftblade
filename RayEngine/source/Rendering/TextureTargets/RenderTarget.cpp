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
    rlActiveDrawBuffers(static_cast<int>(Buffers.size())); // Count - Depth
    for (int i = 0; i < static_cast<int>(Buffers.size()); i++)
    {
        CHECK_ASSERT(!Buffers[i].Tex, "Tex nullptr");
        rlFramebufferAttach(
            FrameBuffer,
            Buffers[i].Tex->id,
            RL_ATTACHMENT_COLOR_CHANNEL0 + i,
            RL_ATTACHMENT_TEXTURE2D,
            0);
    }
    
    uint32 depthID = InRenderTexture.depth.id;
    CHECK_ASSERT(depthID == 0, "Invalid depth texture");
    rlFramebufferAttach(FrameBuffer, depthID, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    
    CHECK_ASSERT(!rlFramebufferComplete(FrameBuffer), "Framebuffer incomplete"); 
}

void RenderTarget::Unload()
{
    for (const Buffer& buff : Buffers)
    {
        CHECK_ASSERT(!buff.Tex, "Tex nullptr");
        rlUnloadTexture(buff.Tex->id);
        delete (buff.Tex);
    }
    Buffers.clear(); 

    if (FrameBuffer)
    {
        rlFramebufferAttach(FrameBuffer, 0, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
        rlUnloadFramebuffer(FrameBuffer);
        FrameBuffer = 0;
    }
    Width = 0;
    Height = 0; 
}

void RenderTarget::BeginWrite(int InBlend, bool InClear) const
{
    rlEnableFramebuffer(FrameBuffer);
    if (InClear)
    {
        rlClearColor(0, 0, 0, 0);
        rlClearScreenBuffers();
    }
    InBlend >= RL_BLEND_ALPHA ?
        rlSetBlendMode(RL_BLEND_ALPHA) : rlDisableColorBlend();
}

void RenderTarget::EndWrite() const
{
    rlDisableFramebuffer();
}

void RenderTarget::Bind(ShaderResource& InShader, Slot& InOutSlots, const String& InPostfix) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);
    for (const auto& buff : Buffers)
    {
        InOutSlots.Index++;
        int loc = InShader.GetLocation(buff.Name + InPostfix);
        CHECK_CONTINUE(loc < 0);
        
        InOutSlots.Loc++;
        CHECK_ASSERT(!buff.Tex, "Tex nullptr");
        
        rlTextureParameters(buff.Tex->id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
        rlTextureParameters(buff.Tex->id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
        rlActiveTextureSlot(InOutSlots.Loc);
        rlEnableTexture(buff.Tex->id);
        rlSetUniform(loc, &InOutSlots.Loc, RL_SHADER_UNIFORM_SAMPLER2D, 1); // NOTE: Default texture is always activated as GL_TEXTURE0
        rlActiveTextureSlot(0);
    }
}

void RenderTarget::Unbind(ShaderResource& InShader, Slot& InOutSlots, const String& InPostfix) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);
    for (const auto& buff : Buffers)
    {
        InOutSlots.Index++;
        int loc = InShader.GetLocation(buff.Name + InPostfix);
        CHECK_CONTINUE(loc < 0);
        
        InOutSlots.Loc++;
        rlActiveTextureSlot(InOutSlots.Loc);
        rlDisableTexture();
        int l = 0;
        rlSetUniform(loc, &l, RL_SHADER_UNIFORM_SAMPLER2D, 1); // NOTE: Default texture is always activated as GL_TEXTURE0
        rlActiveTextureSlot(0);
    }
}

void RenderTarget::CreateBuffer(const String& InName, const uint8 InPixelFormat)
{
    auto& buffer = Buffers.emplace_back();
    buffer.Name = InName;
    buffer.Tex = new Texture(); 
    buffer.Tex->id = rlLoadTexture(nullptr, Width, Height, InPixelFormat, 1); 
    buffer.Tex->width = Width;
    buffer.Tex->height = Height;
    buffer.Tex->mipmaps = 0;
    buffer.Tex->format = InPixelFormat;
}

