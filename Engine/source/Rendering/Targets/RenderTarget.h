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
        friend class CommandList;
        friend class BufferGroup;
    public:
        struct Desc
        {
            Vec3I size = {};
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA16Float;
            TextureType type = TextureType::TEXTURE_2D;
            int multisample = 0;
            
            // Also sampler parameters? 
            // Or should this be passed separately?
            // Texture is always read separately?
        };
        
        // A RenderTarget can be rendered to and read from
        // It has a size and a format
        
        void Init(const Desc& InDesc);
        void Init(const wgpu::Texture& InTexture);
        void Deinit();
        
        Vec3I GetSize() const;

    private:
        wgpu::TextureDimension GetDimension() const;
        wgpu::TextureViewDimension GetViewDimension() const;
        
        wgpu::Texture texture;
        wgpu::TextureView view;
        Desc descriptor;
    };
}