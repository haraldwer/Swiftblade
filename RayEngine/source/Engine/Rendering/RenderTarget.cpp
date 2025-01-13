#include "RenderTarget.h"

#include "Resources/Shader.h"
#include "rlgl.h"

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
        rlFramebufferAttach(
            FrameBuffer,
            Buffers[i].Tex.id,
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
        rlUnloadTexture(buff.Tex.id);
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

void RenderTarget::BeginWrite() const
{
    rlEnableFramebuffer(FrameBuffer);
    rlClearColor(0, 0, 0, 0);
    rlClearScreenBuffers();
    rlDisableColorBlend();
}

void RenderTarget::EndWrite() const
{
    rlDisableFramebuffer();
    rlEnableColorBlend();
}

void RenderTarget::Bind(ShaderResource& InShader, Slot& InOutSlots) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);
    for (const auto& buff : Buffers)
    {
            
        InOutSlots.Index++;
        int loc = InShader.GetLocation(buff.Name);
        CHECK_CONTINUE(loc < 0);
        
        InOutSlots.Loc++;
        rlActiveTextureSlot(InOutSlots.Loc);
        rlEnableTexture(buff.Tex.id);
        rlSetUniform(loc, &InOutSlots.Loc, RL_SHADER_UNIFORM_SAMPLER2D, 1); // NOTE: Default texture is always activated as GL_TEXTURE0
        rlActiveTextureSlot(0);
    }
}

void RenderTarget::Unbind(ShaderResource& InShader, Slot& InOutSlots) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);
    for (const auto& buff : Buffers)
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

void RenderTarget::CreateBuffer(const String& InName, const PixelFormat InPixelFormat)
{
    auto& buffer = Buffers.emplace_back();
    buffer.Name = InName;
    buffer.Tex.id = rlLoadTexture(nullptr, Width, Height, InPixelFormat, 1); 
    buffer.Tex.width = Width;
    buffer.Tex.height = Height;
    buffer.Tex.mipmaps = 1;
    buffer.Tex.format = InPixelFormat;
}

