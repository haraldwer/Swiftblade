#pragma once

#include <webgpu/webgpu.hpp>

namespace Rendering
{
    class RenderTarget
    {
        friend class Window;
    public:
        
        // A RenderTarget can be rendered to and read from
        // It has a size and a format
        
        void Init();
        void Deinit();
        
        wgpu::TextureFormat GetFormat() const;
        wgpu::TextureViewDescriptor GetView() const;
        wgpu::Texture& GetTexture();

    private:
        wgpu::Texture texture;
        
    };
}