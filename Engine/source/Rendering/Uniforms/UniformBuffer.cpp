#include "UniformBuffer.h"

#include "Context/Context.h"
#include "Targets/RenderTarget.h"

void Rendering::BufferGroup::Set(const int InSlot, const void* InData, const uint64 InSize, WGPUShaderStage InVisibility)
{
    CHECK_ASSERT(!InData, "Invalid data");
    CHECK_ASSERT(!InSize, "Invalid size");
    CHECK_ASSERT(InSlot < 0, "Invalid slot");
    
    if (uniforms.size() <= InSlot)
        uniforms.resize(InSlot + 1);
    auto& uniform = uniforms.at(InSlot);
    if (layouts.size() <= InSlot)
        layouts.resize(InSlot + 1);
    auto& layout = layouts.at(InSlot);
    if (bindings.size() <= InSlot)
        bindings.resize(InSlot + 1);
    auto& binding = bindings.at(InSlot);
    if (hashes.size() <= InSlot)
        hashes.resize(InSlot + 1);
    
    // Resize buffer
    bool changed = false;
    if (uniform.size != InSize || !uniform.data)
    {
        if (uniform.data != nullptr) 
            free(uniform.data);
        uniform.data = malloc(InSize);
        uniform.size = InSize;
        
        // Also recreate buffer
        if (uniform.buffer)
            wgpuBufferDestroy(uniform.buffer);
        WGPUBufferDescriptor desc;
        desc.size = InSize;
        desc.usage = WGPUBufferUsage_Uniform;
        desc.label = WGPUStringView("Uniform buffer"); // TODO: Make unique
        uniform.buffer = Context::Get().CreateBuffer(desc);
        
        binding.binding = InSlot;  // Index of bidning
        binding.buffer = uniform.buffer;
        binding.offset = 0; // Offset within buffer
        binding.size = InSize; // Size of buffer
        bindingChanged = true;
        changed = true;
    }
    
    // Copy data
    if (changed || 
        std::memcmp(uniform.data, InData, InSize) != 0)
    {
        std::memcpy(uniform.data, InData, uniform.size);
        Context::Get().WriteBuffer(uniform.buffer, uniform.data, uniform.size);
    }
    
    // Update layout
    if (changed || 
        layout.visibility != InVisibility || 
        layout.binding != InSlot)
    {
        layout.binding = InSlot; // Index in shader
        layout.visibility = InVisibility;
        layout.buffer.type = WGPUBufferBindingType_Uniform;
        layout.buffer.minBindingSize = InSize;
        layout.buffer.hasDynamicOffset = WGPUOptionalBool_False;
        // TODO: Textures are bound here ^
        layoutChanged = true;
        
        // Slot part of hash order
        struct LayoutHash
        {
            uint64 size;
            WGPUShaderStage visibility;
        } hashData {
            InSize,
            InVisibility
        };
        hashes.at(InSlot) = Utility::Hash(hashData);
    }
}

void Rendering::BufferGroup::Set(const int InSlot, const WGPUTextureView& InView, const WGPUTextureBindingLayout& InLayout, WGPUShaderStage InVisibility)
{
    CHECK_ASSERT(!InView, "Invalid texture");
    CHECK_ASSERT(InSlot < 0, "Invalid slot");
    
    if (static_cast<int>(uniforms.size()) <= InSlot)
        uniforms.resize(InSlot + 1);
    auto& uniform = uniforms.at(InSlot);
    if (static_cast<int>(layouts.size()) <= InSlot)
        layouts.resize(InSlot + 1);
    auto& layout = layouts.at(InSlot);
    if (static_cast<int>(bindings.size()) <= InSlot)
        bindings.resize(InSlot + 1);
    auto& binding = bindings.at(InSlot);
    if (static_cast<int>(hashes.size()) <= InSlot)
        hashes.resize(InSlot + 1);
    
    // Resize buffer
    bool changed = false;
    if (uniform.view != InView || uniform.data || uniform.size > 0) // Did uniform change?
    {
        if (uniform.data)
        {
            free(uniform.data);
            uniform.data = nullptr;
        }
        uniform.size = 0;
        uniform.view = InView;
        binding.binding = InSlot;  // Index of bidning
        binding.textureView = InView;
        bindingChanged = true;
        changed = true;
    }
    
    // Update layout
    if (changed || 
        layout.visibility != InVisibility || 
        layout.binding != InSlot)
    {
        layout.binding = InSlot;
        layout.visibility = InVisibility;
        layout.texture = InLayout;
        layoutChanged = true;
        
        // Slot part of hash order
        struct LayoutHash
        {
            WGPUTextureBindingLayout layout;
            WGPUShaderStage visibility;
        } hashData {
            InLayout,
            InVisibility
        };
        hashes.at(InSlot) = Utility::Hash(hashData);
    }
}

void Rendering::BufferGroup::Set(const int InSlot, const RenderTarget &InTexture, const WGPUShaderStage InVisibility)
{
    WGPUTextureBindingLayout layout;
    layout.sampleType = WGPUTextureSampleType_Float;
    if (InTexture.descriptor.type == TextureType::DEPTH)
        layout.sampleType = WGPUTextureSampleType_Depth;
    else if (InTexture.descriptor.multisample > 1)
        layout.sampleType = WGPUTextureSampleType_UnfilterableFloat;
    layout.multisampled = InTexture.descriptor.multisample > 1 ?
        WGPUOptionalBool_True : WGPUOptionalBool_False;
    layout.viewDimension = InTexture.GetViewDimension();
    Set(InSlot, InTexture.view, layout, InVisibility);
}

void Rendering::BufferGroup::Clear()
{
    for (auto& uniform : uniforms)
    {
        if (uniform.data)
        {
            free(uniform.data);
            uniform.data = nullptr;
        }
        if (uniform.buffer)
            wgpuBufferDestroy(uniform.buffer);
    }
    uniforms.clear();
}

Rendering::BufferGroup & Rendering::BufferCollection::GetGroup(const int InSlot)
{
    if (groups.size() <= InSlot)
        groups.resize(InSlot + 1);
    return groups.at(InSlot);
}

Rendering::PipelineLayout* Rendering::BufferCollection::GetLayout()
{
    CHECK_RETURN(groups.empty(), nullptr)
    CHECK_ASSERT(layouts.size() > groups.size(), "Invalid layout size");
    CHECK_ASSERT(bindings.size() > groups.size(), "Invalid binding size");
    layouts.resize(groups.size());
    bindings.resize(groups.size());
    
    // Recreate binding and layout for any group that changed
    bool changed = false;
    for (int i = 0; i < static_cast<int>(groups.size()); i++)
    {
        auto& group = groups.at(i);
        auto& groupLayout = layouts.at(i);
        auto& groupBinding = bindings.at(i);
        if (!group.bindingChanged && !group.layoutChanged)
            continue;
        
        // Recreate layout
        if (group.layoutChanged)
        {
            if (groupLayout)
                wgpuBindGroupLayoutRelease(groupLayout);
            groupLayout = Context::Get().CreateBindGroupLayout(group.layouts);
        }
        
        // Recreate binding (depends on layout)
        if (groupBinding)
            wgpuBindGroupRelease(groupBinding);
        groupBinding = Context::Get().CreateBindGroup(groupLayout, group.bindings);
        
        changed = true;
        group.bindingChanged = false;
        group.layoutChanged = false;
    }
    
    if (changed)
    {
        // Recreate final pipeline layout
        if (layout.layout)
            wgpuPipelineLayoutRelease(layout.layout);
        layout.layout = Context::Get().CreateLayout(layouts);
        
        // Collect hashes
        Vector<uint32> hashes;
        hashes.reserve(groups.size() * 5);
        for (auto& g : groups)
            hashes.insert(hashes.end(), g.hashes.begin(), g.hashes.end());
        layout.hash = Utility::Hash(hashes);
    }
    
    return &layout;
}

void Rendering::BufferCollection::Clear()
{
    for (auto& group : groups)
        group.Clear();
    groups.clear();
    for (auto& groupLayout : layouts)
        wgpuBindGroupLayoutRelease(groupLayout);
    for (auto& groupBinding : bindings)
        wgpuBindGroupRelease(groupBinding);
    layouts.clear();
    bindings.clear();
    if (layout.layout)
    {
        wgpuPipelineLayoutRelease(layout.layout);
        layout = {};
    }
}

void Rendering::BufferCollection::Bind(const WGPURenderPassEncoder& InEncoder) const
{
    CHECK_ASSERT(!InEncoder, "Invalid encoder");
    for (int i = 0; i < bindings.size(); i++)
        if (auto& binding = bindings.at(i))
            wgpuRenderPassEncoderSetBindGroup(InEncoder, i, binding, 0, nullptr);
}
