#pragma once

#include <webgpu/webgpu.hpp>

namespace Rendering
{
    enum class TextureType
    {
        TEXTURE_2D,
        TEXTURE_1D,
        TEXTURE_3D,
        CUBEMAP,
        DEPTH,
    };
    
    class RenderTarget
    {
        friend class Window;
    public:
        struct Desc
        {
            Vec3I size = {};
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA16Float;
            TextureType type = TextureType::TEXTURE_2D;
            bool multisample = false;
        };
        
        // A RenderTarget can be rendered to and read from
        // It has a size and a format
        
        void Init(const Desc& InDesc);
        void Init(const wgpu::Texture& InTexture);
        void Deinit();
        
        wgpu::TextureFormat GetFormat() const;
        wgpu::TextureView GetView() const;
        wgpu::TextureBindingLayout GetLayout() const;
        wgpu::Texture GetTexture() const;
        Vec3I GetSize() const;
        uint32 GetHash() const;

    private:
        wgpu::TextureDimension GetDimension() const;
        wgpu::TextureViewDimension GetViewDimension() const;
        
        wgpu::Texture texture;
        wgpu::TextureView view;
        Desc descriptor;
        uint32 hash; 
    };
}