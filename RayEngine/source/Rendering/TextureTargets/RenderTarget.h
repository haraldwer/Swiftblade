#pragma once

struct RenderTexture;
struct Texture;

namespace Rendering
{
    class ShaderResource;
    
    enum class TexType
    {
        TEX,
        TEX_3D,
        CUBEMAP,
    };
    
    class RenderTarget
    {
    public:
        
        struct TargetTex
        {
            Texture* tex = {};
            String name = {}; 
            TexType type;
            int defaultFilter = -1;
            Vec3I scaledSize;
        };
        
        bool Setup(const Vec3I& InRes, const String& InName, uint8 InFormat, int InDefaultFilter = -1, TexType InType = TexType::TEX);
        bool TryBeginSetup(const Vec3I& InRes);
        
        void CreateBuffer(const String& InName, uint8 InPixelFormat, float InResScale = 1.0f, int InDefaultFilter = -1, int InMips = 1, TexType InType = TexType::TEX);
        void AttachDepth(const RenderTexture& InTarget) const;
        void EndSetup() const;
        
        void Unload();
        
        void Attach(int InLayerFace) const;
        void Bind(ShaderResource& InShader, int& InOutSlot, int InFilter = -1, const String& InPostfix = "") const;
        
        const Vector<TargetTex>& GetTextures() const { return textures; }
        int NumTextures() const { return static_cast<int>(textures.size()); }
        Vec2I Size() const { return size.xy; }
        uint32 GetFBO() const { return frameBuffer; }
        bool IsValid() const { return frameBuffer != 0 && size.x != 0 && size.y != 0 && !textures.empty(); }

    private: 
        
        uint32 frameBuffer = 0;
        Vector<TargetTex> textures = {};
        Vec3I size;
    };
}
