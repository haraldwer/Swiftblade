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
            Texture* tex = {};
            String name = {}; 
            bool cubemap = false;
        };
        
        bool Setup(const RenderTexture& InTarget, const String& InName, uint8 InFormat);
        bool TryBeginSetup(const RenderTexture& InRenderTexture);
        void CreateBuffer(const String& InName, uint8 InPixelFormat, float InResScale = 1.0f, int InMips = 1, bool InCubemap = false);
        void EndSetup(const RenderTexture& InRenderTexture) const;
        void Unload();
        void Bind(ShaderResource& InShader, int& InOutSlot, int InFilter = -1, const String& InPostfix = "") const;
        
        const Vector<TargetTex>& GetTextures() const { return textures; }
        Vec2I Size() const { return { width, height }; }
        uint32 GetFBO() const { return frameBuffer; }
        
    private: 
        
        uint32 frameBuffer = 0;
        Vector<TargetTex> textures = {};

        int width = 0;
        int height = 0;
    };
}
