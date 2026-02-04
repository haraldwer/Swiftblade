#include "RenderTarget.h"

#include "Context/Context.h"

void Rendering::RenderTarget::Init(const Desc &InDesc)
{
    descriptor = InDesc;
    hash = Utility::Hash(descriptor);
    
    // Clamp layer count
    if (descriptor.type != TextureType::TEXTURE_3D)
        descriptor.size.z = 1;
    if (descriptor.type == TextureType::CUBEMAP)
        descriptor.size.z = 6;
    
    wgpu::TextureDescriptor desc;
    desc.size.width = descriptor.size.x;
    desc.size.height = descriptor.size.y;
    desc.size.depthOrArrayLayers = descriptor.size.z;
    desc.dimension = GetDimension();
    desc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
    desc.format = descriptor.format;
    desc.viewFormatCount = 0;
    desc.viewFormats = nullptr;
    desc.sampleCount = 1;
    desc.mipLevelCount = 1;
    texture = Context::Get().CreateTexture(desc);
    
    wgpu::TextureViewDescriptor viewDesc;
    viewDesc.nextInChain = nullptr;
    viewDesc.label = wgpu::StringView("RenderTarget texture view");
    viewDesc.format = descriptor.format;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = descriptor.size.z;
    viewDesc.aspect = descriptor.type == TextureType::DEPTH ? 
         wgpu::TextureAspect::DepthOnly : wgpu::TextureAspect::All;
    viewDesc.dimension = GetViewDimension();
    view = texture.createView(viewDesc);
    CHECK_ASSERT(!view, "Failed to create view");
}

void Rendering::RenderTarget::Init(const wgpu::Texture &InTexture)
{
    CHECK_ASSERT(!InTexture, "Invalid texture");
    
    descriptor.size = {
        static_cast<int>(InTexture.getWidth()),
        static_cast<int>(InTexture.getHeight()),
        static_cast<int>(InTexture.getDepthOrArrayLayers())
    };
    descriptor.format = InTexture.getFormat();
    descriptor.multisample = InTexture.getSampleCount() > 0;
    descriptor.type = TextureType::TEXTURE_2D; // Assume tex2D
    
    texture = InTexture;
    
    wgpu::TextureViewDescriptor viewDesc;
    viewDesc.nextInChain = nullptr;
    viewDesc.label = wgpu::StringView("RenderTarget texture view");
    viewDesc.format = descriptor.format;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = descriptor.size.z;
    viewDesc.aspect = wgpu::TextureAspect::All;
    viewDesc.dimension = GetViewDimension();
    view = texture.createView(viewDesc);
    CHECK_ASSERT(!view, "Failed to create view");
}

void Rendering::RenderTarget::Deinit()
{
    if (texture)
        texture.release();
    if (view)
        view.release();
    descriptor = {};
    hash = 0;
}

wgpu::TextureView Rendering::RenderTarget::GetView() const
{
    return view;
}

wgpu::TextureBindingLayout Rendering::RenderTarget::GetLayout() const
{
    wgpu::TextureBindingLayout layout;
    layout.sampleType = descriptor.type == TextureType::DEPTH ?  
        wgpu::TextureSampleType::Depth : wgpu::TextureSampleType::Float;
    layout.multisampled = descriptor.multisample ?
        wgpu::OptionalBool::True : wgpu::OptionalBool::False;
    layout.viewDimension = GetViewDimension();
    return layout;
}

wgpu::Texture Rendering::RenderTarget::GetTexture() const
{
    return texture;
}

uint32 Rendering::RenderTarget::GetHash() const
{
    return hash;
}

wgpu::TextureDimension Rendering::RenderTarget::GetDimension() const
{
    switch (descriptor.type)
    {
        case TextureType::TEXTURE_2D: return wgpu::TextureDimension::_2D;
        case TextureType::TEXTURE_1D: return wgpu::TextureDimension::_1D;
        case TextureType::TEXTURE_3D: return wgpu::TextureDimension::_3D;
        case TextureType::CUBEMAP: return wgpu::TextureDimension::_2D;
        case TextureType::DEPTH: return wgpu::TextureDimension::_2D;
    }
    return wgpu::TextureDimension::Undefined;
}

wgpu::TextureViewDimension Rendering::RenderTarget::GetViewDimension() const
{
    switch (descriptor.type)
    {
        case TextureType::TEXTURE_2D: return wgpu::TextureViewDimension::_2D;
        case TextureType::TEXTURE_1D: return wgpu::TextureViewDimension::_1D;
        case TextureType::TEXTURE_3D: return wgpu::TextureViewDimension::_3D;
        case TextureType::CUBEMAP: return wgpu::TextureViewDimension::_2DArray;
        case TextureType::DEPTH: return wgpu::TextureViewDimension::_2D;
    }
    return wgpu::TextureViewDimension::Undefined;
}

Vec3I Rendering::RenderTarget::GetSize() const
{
    return descriptor.size;
}

wgpu::TextureFormat Rendering::RenderTarget::GetFormat() const
{
    return descriptor.format;
}
