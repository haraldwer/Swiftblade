#include "UniformBuffer.h"

#include "Context/Context.h"

void Rendering::BufferGroup::Set(const int InSlot, const void* InData, const uint64 InSize, wgpu::ShaderStage InVisibility)
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
            uniform.buffer.release();
        wgpu::BufferDescriptor desc;
        desc.size = InSize;
        desc.usage = wgpu::BufferUsage::Uniform;
        desc.label = wgpu::StringView("Uniform buffer"); // TODO: Make unique
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
        layout.buffer.type = wgpu::BufferBindingType::Uniform;
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
            uniform.buffer.release();
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
                groupLayout.release();
            groupLayout = Context::Get().CreateBindGroupLayout(group.layouts);
        }
        
        // Recreate binding (depends on layout)
        if (groupBinding)
            groupBinding.release();
        groupBinding = Context::Get().CreateBindGroup(groupLayout, group.bindings);
        
        changed = true;
        group.bindingChanged = false;
        group.layoutChanged = false;
    }
    
    if (changed)
    {
        // Recreate final pipeline layout
        if (layout.layout)
            layout.layout.release();
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
        groupLayout.release();
    for (auto& groupBinding : bindings)
        groupBinding.release();
    layout.layout.release();
    layout = {};
}

void Rendering::BufferCollection::Bind(const wgpu::RenderPassEncoder& InEncoder) const
{
    CHECK_ASSERT(!InEncoder, "Invalid encoder");
    for (int i = 0; i < bindings.size(); i++)
        if (auto& binding = bindings.at(i))
            InEncoder.setBindGroup(i, binding, 0, nullptr);
}
