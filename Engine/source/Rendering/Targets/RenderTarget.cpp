#include "RenderTarget.h"

void Rendering::RenderTarget::Init()
{
    
}

void Rendering::RenderTarget::Deinit()
{
    
}

wgpu::TextureViewDescriptor Rendering::RenderTarget::GetView() const
{
    // This depends on the target settings
    // Maybe this should be 
    wgpu::TextureViewDescriptor viewDesc;
    viewDesc.nextInChain = nullptr;
    viewDesc.label = wgpu::StringView("Surface texture view");
    viewDesc.format = GetFormat();
    viewDesc.dimension = wgpu::TextureViewDimension::_2D;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = wgpu::TextureAspect::All;
    return viewDesc;
}

wgpu::Texture & Rendering::RenderTarget::GetTexture()
{
    return texture;
}

uint32 Rendering::RenderTarget::GetHash()
{
    // Based on parameters?
    return 0;
}

wgpu::TextureFormat Rendering::RenderTarget::GetFormat() const
{
    if (texture)
        return wgpuTextureGetFormat(texture);
    return wgpu::TextureFormat::Undefined;
}
