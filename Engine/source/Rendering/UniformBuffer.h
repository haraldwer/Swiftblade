#pragma once
#include <webgpu/webgpu.hpp>

namespace Rendering
{
    struct PipelineLayout
    {
        wgpu::PipelineLayout layout;
        uint32 hash = 0;
    };
    
    class BufferGroup
    {
        friend class BufferCollection;
    public:
        
        template <class T>
        void Set(const int InSlot, const T& InData, wgpu::ShaderStage InVisibility = wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex)
        {
            Set(InSlot, reinterpret_cast<const void *>(&InData), sizeof(T), InVisibility);
        }
        
        template <class T>
        void Set(const int InSlot, const Vector<T>& InData, wgpu::ShaderStage InVisibility = wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex)
        {
            Set(InSlot, reinterpret_cast<const void*>(InData.data()), InData.size() * sizeof(T), InVisibility);
        }
        
        void Set(int InSlot, const void* InData, uint64 InSize, wgpu::ShaderStage InVisibility = wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex);
        
        void Clear();
        
    private:
        struct Uniform
        {
            void* data = nullptr;
            uint64 size = 0;
            wgpu::Buffer buffer;
        };
        Vector<Uniform> uniforms;
        Vector<wgpu::BindGroupLayoutEntry> layouts;
        Vector<wgpu::BindGroupEntry> bindings;
        Vector<uint32> hashes;
        
        bool bindingChanged = false;
        bool layoutChanged = false;
    };
    
    class BufferCollection
    {
    public:
        
        BufferGroup& GetGroup(int InSlot);
        PipelineLayout* GetLayout();
        void Clear();
        void Bind(const wgpu::RenderPassEncoder& InEncoder) const;

    private:
        Vector<BufferGroup> groups;
        Vector<wgpu::BindGroupLayout> layouts;
        Vector<wgpu::BindGroup> bindings;
        PipelineLayout layout;
    };
}
