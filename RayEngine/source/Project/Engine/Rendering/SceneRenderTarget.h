#pragma once

// A collection of render targets
// Basically GBuffer
class SceneRenderTarget
{
    struct Buffer
    {
        Texture2D Tex = {};
        String Name; 
    };
    
public:
    void TrySetup(const RenderTexture& InRenderTexture);
    void Unload();
    void BeginWrite() const;
    void EndWrite() const;
    void Bind(const Shader& InShader, int InStartSlot) const;
    void BlitDepth() const;
    
    const Vector<Buffer>& GetBuffers() const { return Buffers; }

private: 

    void CreateBuffer(const String& InName, PixelFormat InPixelFormat);
    
    uint32 FrameBuffer = 0;
    uint32 DepthBuffer = 0; 
    
    Vector<Buffer> Buffers;

    int Width = 0;
    int Height = 0;
};
