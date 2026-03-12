#include "RenderTarget.h"

#include "Context/Context.h"

void Rendering::RenderTarget::Init(const Desc &InDesc)
{
    descriptor = InDesc;
    
    // Clamp layer count
    if (descriptor.type != TextureType::TEXTURE_3D)
        descriptor.size.z = 1;
    if (descriptor.type == TextureType::CUBEMAP)
        descriptor.size.z = 6;
    
    WGPUTextureDescriptor desc = {};
    desc.size.width = descriptor.size.x;
    desc.size.height = descriptor.size.y;
    desc.size.depthOrArrayLayers = descriptor.size.z;
    desc.dimension = GetDimension();
    desc.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
    desc.format = descriptor.format;
    desc.sampleCount = Utility::Math::Clamp(descriptor.multisample, 1, 8);
    desc.mipLevelCount = 1;
    texture = Context::Get().CreateTexture(desc);
    
    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.label = WGPUStringView("RenderTarget texture view");
    viewDesc.format = descriptor.format;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = descriptor.size.z;
    viewDesc.aspect = descriptor.type == TextureType::DEPTH ? 
         WGPUTextureAspect_DepthOnly : WGPUTextureAspect_All;
    viewDesc.dimension = GetViewDimension();
    view = wgpuTextureCreateView(texture, &viewDesc);
    CHECK_ASSERT(!view, "Failed to create view");
}

void Rendering::RenderTarget::Init(const WGPUTexture& InTexture)
{
    CHECK_ASSERT(!InTexture, "Invalid texture");
    
    descriptor.size = {
        static_cast<int>(wgpuTextureGetWidth(InTexture)),
        static_cast<int>(wgpuTextureGetHeight(InTexture)),
        static_cast<int>(wgpuTextureGetDepthOrArrayLayers(InTexture))
    };
    descriptor.format = wgpuTextureGetFormat(InTexture);
    descriptor.multisample = wgpuTextureGetSampleCount(InTexture) > 0;
    descriptor.type = TextureType::TEXTURE_2D; // Assume tex2D
    
    texture = InTexture;
    
    WGPUTextureViewDescriptor viewDesc = {};
    viewDesc.label = WGPUStringView("RenderTarget texture view");
    viewDesc.format = descriptor.format;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = descriptor.size.z;
    viewDesc.aspect = WGPUTextureAspect_All;
    viewDesc.dimension = GetViewDimension();
    view = wgpuTextureCreateView(texture, &viewDesc);
    CHECK_ASSERT(!view, "Failed to create view");
}

void Rendering::RenderTarget::Deinit()
{
    if (view)
        wgpuTextureViewRelease(view);
    if (texture)
        wgpuTextureDestroy(texture);
    view = {};
    texture = {};
    descriptor = {};
}

WGPUTextureDimension Rendering::RenderTarget::GetDimension() const
{
    switch (descriptor.type)
    {
        case TextureType::TEXTURE_2D: return WGPUTextureDimension_2D;
        case TextureType::TEXTURE_1D: return WGPUTextureDimension_1D;
        case TextureType::TEXTURE_3D: return WGPUTextureDimension_3D;
        case TextureType::CUBEMAP: return WGPUTextureDimension_2D;
        case TextureType::DEPTH: return WGPUTextureDimension_2D;
    }
    return WGPUTextureDimension_Undefined;
}

WGPUTextureViewDimension Rendering::RenderTarget::GetViewDimension() const
{
    switch (descriptor.type)
    {
        case TextureType::TEXTURE_2D: return WGPUTextureViewDimension_2D;
        case TextureType::TEXTURE_1D: return WGPUTextureViewDimension_1D;
        case TextureType::TEXTURE_3D: return WGPUTextureViewDimension_3D;
        case TextureType::CUBEMAP: return WGPUTextureViewDimension_2DArray;
        case TextureType::DEPTH: return WGPUTextureViewDimension_2D;
    }
    return WGPUTextureViewDimension_Undefined;
}

Vec3I Rendering::RenderTarget::GetSize() const
{
    return descriptor.size;
}
