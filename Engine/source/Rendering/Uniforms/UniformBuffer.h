#pragma once
#include "webgpu/webgpu.h"

namespace Rendering
{
    class RenderTarget;

    struct PipelineLayout
    {
        WGPUPipelineLayout layout = {};
        uint32 hash = 0;
    };
    
    class BufferGroup
    {
        friend class BufferCollection;
    public:
        
        template <class T>
        void Set(const int InSlot, const T& InData, WGPUShaderStage InVisibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex)
        {
            Set(InSlot, reinterpret_cast<const void *>(&InData), sizeof(T), InVisibility);
        }
        
        template <class T>
        void Set(const int InSlot, const Vector<T>& InData, WGPUShaderStage InVisibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex)
        {
            Set(InSlot, reinterpret_cast<const void*>(InData.data()), InData.size() * sizeof(T), InVisibility);
        }
        
        void Set(int InSlot, const void* InData, uint64 InSize, WGPUShaderStage InVisibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex);
        void Set(int InSlot, const WGPUTextureView& InView, const WGPUTextureBindingLayout& InLayout, WGPUShaderStage InVisibility = WGPUShaderStage_Fragment);
        void Set(int InSlot, const RenderTarget& InTexture, WGPUShaderStage InVisibility = WGPUShaderStage_Fragment);
        
        void Clear();
        
    private:
        
        struct Uniform
        {
            void* data = nullptr;
            uint64 size = 0;
            WGPUBuffer buffer = {};
            WGPUTextureView view = {};
        };
        Vector<Uniform> uniforms;
        Vector<WGPUBindGroupLayoutEntry> layouts;
        Vector<WGPUBindGroupEntry> bindings;
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
        void Bind(const WGPURenderPassEncoder& InEncoder) const;

    private:
        Vector<BufferGroup> groups;
        Vector<WGPUBindGroupLayout> layouts;
        Vector<WGPUBindGroup> bindings;
        PipelineLayout layout = {};
    };
}
