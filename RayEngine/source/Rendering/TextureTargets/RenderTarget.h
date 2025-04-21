#pragma once

class ShaderResource;
struct RenderTexture;
struct Texture;

namespace Rendering
{
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
        void CreateBuffer(const String& InName, uint8 InPixelFormat, float InResScale = 1.0f, int InMips = 1, bool InCubemap = false);
        void EndSetup(const RenderTexture& InRenderTexture) const;
        void Unload();
        void Bind(ShaderResource& InShader, int& InOutSlot, const String& InPostfix = "") const;
        
        const Vector<TargetTex>& GetTextures() const { return Textures; }
        Vec2F Size() const { return { static_cast<float>(Width), static_cast<float>(Height) }; }
        uint32 GetFBO() const { return FrameBuffer; }
        
    private: 
        
        uint32 FrameBuffer = 0;
        Vector<TargetTex> Textures;

        int Width = 0;
        int Height = 0;
    };
}
