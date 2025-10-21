#pragma once

struct RenderTexture;
struct Texture;

namespace Rendering
{
    class ShaderResource;
    
    class RenderTarget
    {
    public:
        struct TargetTex
        {
            Texture* tex = {};
            String name = {}; 
            bool cubemap = false;
            int defaultFilter = -1;
            int scaledWidth = 0;
            int scaledHeight = 0;
        };
        
        bool Setup(const Vec2I& InRes, const String& InName, uint8 InFormat, int InDefaultFilter = -1);
        bool TryBeginSetup(const Vec2I& InRes);
        void CreateBuffer(const String& InName, uint8 InPixelFormat, float InResScale = 1.0f, int InDefaultFilter = -1, int InMips = 1, bool InCubemap = false);
        void EndSetup() const;
        
        void AttachDepth(const RenderTexture& InTarget) const;
        
        void Unload();
        void Bind(ShaderResource& InShader, int& InOutSlot, int InFilter = -1, const String& InPostfix = "") const;
        
        const Vector<TargetTex>& GetTextures() const { return textures; }
        int NumTextures() const { return static_cast<int>(textures.size()); }
        Vec2I Size() const { return { width, height }; }
        uint32 GetFBO() const { return frameBuffer; }
        bool IsValid() const { return frameBuffer != 0 && height != 0 && width != 0 && !textures.empty(); }

    private: 
        
        uint32 frameBuffer = 0;
        uint32 depthID = 0;
        Vector<TargetTex> textures = {};

        int width = 0;
        int height = 0;
    };
}
