#pragma once

class ShaderResource;
struct RenderTexture;
struct Texture;

class RenderTarget
{
    struct Buffer
    {
        Texture* Tex = {};
        String Name; 
    };
    
public:
    
    bool Setup(const RenderTexture& InTarget, const String& InName, uint8 InFormat);
    
    bool TryBeginSetup(const RenderTexture& InRenderTexture);
    void CreateBuffer(const String& InName, uint8 InPixelFormat);
    void EndSetup(const RenderTexture& InRenderTexture) const;
    
    void Unload();
    void BeginWrite(int InBlend = -1, bool InClear = true) const;
    void EndWrite() const;

    struct Slot
    {
        int Loc = 0;
        int Index = 0;
    };
    
    void Bind(ShaderResource& InShader, Slot& InOutSlots, const String& InPostfix = "") const;
    void Unbind(ShaderResource& InShader, Slot& InOutSlots, const String& InPostfix = "") const;
    
    const Vector<Buffer>& GetBuffers() const { return Buffers; }
    Vec2F Size() const { return { static_cast<float>(Width), static_cast<float>(Height) }; }
    
private: 
    
    uint32 FrameBuffer = 0;
    Vector<Buffer> Buffers;

    int Width = 0;
    int Height = 0;
};
