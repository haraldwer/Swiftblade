#pragma once

#include <webgpu/webgpu.hpp>

#include "Mesh.h"
#include "Utility/Singelton.h"

namespace Rendering
{
    class ModelResource;
    struct MeshState;
    class RenderTarget;
    class MaterialResource;
    struct Command;

    struct PipelineDescriptor
    {
        String label;
        
        MaterialResource* material = nullptr;
        const MeshState* meshState = nullptr;
        Vector<wgpu::TextureFormat> targetFormats;
        
        struct StaticData // Can be hashed directly
        {
            struct DepthData
            {
                wgpu::TextureFormat format = wgpu::TextureFormat::Undefined;
                bool write = true;
            } depth;
            bool multisampling = false;
        } data;
    };
    
    class PipelineCache : public Utility::Singelton<PipelineCache, true>
    {
    public:
        void Init();
        void Deinit();
        
        wgpu::RenderPipeline* GetPipeline(const PipelineDescriptor& InData);
        
    private:
        static bool CreatePipeline(const PipelineDescriptor& InData, uint32 InHash, wgpu::RenderPipeline& OutPipeline);
        
        Map<uint32, wgpu::RenderPipeline> cache;
        // TODO: Also track lifetime!
        // TODO: And load from file!
    };
}
