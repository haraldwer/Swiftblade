#pragma once

class ShaderResource;
struct RenderTexture;
struct Texture;

class RenderTarget
{
public:
    
    struct TargetTex
    {
        Texture* Tex = {};
        String Name; 
        bool Cubemap = false;
    };
    
    bool Setup(const RenderTexture& InTarget, const String& InName, uint8 InFormat);
    
    bool TryBeginSetup(const RenderTexture& InRenderTexture);
    void CreateBuffer(const String& InName, uint8 InPixelFormat, bool InCubemap = false);
    void EndSetup(const RenderTexture& InRenderTexture) const;
    
    void Unload();
    void BeginWrite(int InBlend = -1, bool InClear = true) const;
    void EndWrite() const;

    struct Slot
    {
        int Loc = 0;
        int Index = 0;
    };
    
    void Bind(ShaderResource& InShader, Slot& InOutSlots) const;
    void Unbind(ShaderResource& InShader, Slot& InOutSlots) const;
    
    const Vector<TargetTex>& GetTextures() const { return Textures; }
    Vec2F Size() const { return { static_cast<float>(Width), static_cast<float>(Height) }; }
    
private: 
    
    uint32 FrameBuffer = 0;
    Vector<TargetTex> Textures;

    int Width = 0;
    int Height = 0;
};
