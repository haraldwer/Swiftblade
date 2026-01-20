#pragma once
#include "Interface/Types.h"

namespace Rendering
{
    class ShaderResource;
    
    class RenderTarget
    {
    public:
        
        struct TargetTex
        {
            String name = {}; 
            uint32 texture;
            Vector<uint32> views;
            Vec3I size;
            Vec3I scaledSize;
            TextureParamValue defaultFilter = TextureParamValue::NONE;
            int layerFace = -1;
        };
        
        bool Setup(const Vec3I& InRes, const String& InName, TextureFormat InFormat = TextureFormat::RGBA16, TextureParamValue InDefaultFilter = TextureParamValue::NONE, TextureType InType = TextureType::TEXTURE);
        bool TryBeginSetup(const Vec3I& InRes, TextureType InType = TextureType::TEXTURE);
        
        void CreateBuffer(const String& InName, TextureFormat InFormat = TextureFormat::RGBA16, float InResScale = 1.0f, TextureParamValue InDefaultFilter = TextureParamValue::NONE, int InMips = 1);
        void AttachDepth(const uint32& InTarget) const;
        void EndSetup();
        
        void Unload();
        
        void Attach(int InLayerFace);
        void Bind(ShaderResource& InShader, int& InOutSlot, TextureParamValue InFilter = TextureParamValue::NONE, int InLayerFace = -1, const String& InPostfix = "") const;
        
        const Vector<TargetTex>& GetTextures() const { return textures; }
        int NumTextures() const { return static_cast<int>(textures.size()); }
        Vec2I Size() const { return size.xy; }
        uint32 GetFBO() const { return frameBuffer; }
        bool IsValid() const { return frameBuffer != 0 && size.x != 0 && size.y != 0 && !textures.empty(); }

    private: 
        
        TextureType type = TextureType::TEXTURE;
        uint32 frameBuffer = 0;
        Vector<TargetTex> textures = {};
        Vec3I size;
    };
}
