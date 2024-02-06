#include "SceneRenderTarget.h"

#include "rlgl.h"

void SceneRenderTarget::TrySetup(const RenderTexture& InRenderTexture)
{
    if (Width == InRenderTexture.texture.width &&
        Height == InRenderTexture.texture.height)
        return;

    Unload(); 
    
    Width = InRenderTexture.texture.width;
    Height = InRenderTexture.texture.height;

    // Create framebuffer
    FrameBuffer = rlLoadFramebuffer(Width, Height);
    CHECK_ASSERT(!FrameBuffer, "Unable to create framebuffer"); 
    rlEnableFramebuffer(FrameBuffer);
    
    // Create the textures
    CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
    CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    CreateBuffer("TexColor", PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
    
    // Attach to framebuffer
    rlActiveDrawBuffers(static_cast<int>(Buffers.size())); // Count - Depth
    for (int i = 0; i < static_cast<int>(Buffers.size()); i++)
        rlFramebufferAttach(
            FrameBuffer,
            Buffers[i].Tex.id,
            RL_ATTACHMENT_COLOR_CHANNEL0 + i,
            RL_ATTACHMENT_TEXTURE2D,
            0); 

    int depthID = InRenderTexture.depth.id;
    CHECK_ASSERT(depthID == 0, "Invalid depth texture");
    rlFramebufferAttach(FrameBuffer, depthID, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    
    CHECK_ASSERT(!rlFramebufferComplete(FrameBuffer), "Framebuffer incomplete"); 
}

void SceneRenderTarget::Unload()
{
    for (const Buffer& buff : Buffers)
        rlUnloadTexture(buff.Tex.id);
    Buffers.clear(); 

    rlFramebufferAttach(FrameBuffer, 0, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    rlUnloadFramebuffer(FrameBuffer);
    FrameBuffer = 0;
    Width = 0;
    Height = 0; 
}

void SceneRenderTarget::BeginWrite() const
{
    rlEnableFramebuffer(FrameBuffer);
    rlClearColor(0, 0, 0, 0);
    rlClearScreenBuffers(); 
    rlDisableColorBlend();
}

void SceneRenderTarget::EndWrite() const
{
    rlEnableColorBlend();
    rlDisableFramebuffer();
}

void SceneRenderTarget::Bind(const Shader& InShader, int InStartSlot) const
{
    for (int i = 0; i < Buffers.size(); i++)
    {
        auto& buff = Buffers[i]; 
        rlSetUniformSampler(rlGetLocationUniform(InShader.id, buff.Name.c_str()), InStartSlot + i);
        rlActiveTextureSlot(InStartSlot + i);
        rlEnableTexture(buff.Tex.id);
    }
}

void SceneRenderTarget::CreateBuffer(const String& InName, const PixelFormat InPixelFormat)
{
    auto& buffer = Buffers.emplace_back();
    buffer.Name = InName;
    buffer.Tex.id = rlLoadTexture(nullptr, Width, Height, InPixelFormat, 1); 
    buffer.Tex.width = Width;
    buffer.Tex.height = Height;
    buffer.Tex.mipmaps = 1;
    buffer.Tex.format = InPixelFormat;
}

