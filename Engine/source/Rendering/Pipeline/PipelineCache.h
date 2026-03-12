#pragma once

#include "webgpu/webgpu.h"

#include "Mesh.h"
#include "Utility/Singelton.h"

namespace Rendering
{
    struct PipelineLayout;
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
        Vector<WGPUTextureFormat> targetFormats;
        PipelineLayout* layout = nullptr;
        
        struct StaticData // Can be hashed directly
        {
            struct DepthData
            {
                WGPUTextureFormat format = WGPUTextureFormat_Undefined;
                bool write = true;
            } depth;
            int multisampling = 0;
        } data;
    };
    
    class PipelineCache : public Utility::Singelton<PipelineCache, true>
    {
    public:
        void Init();
        void Deinit();
        
        WGPURenderPipeline* GetPipeline(const PipelineDescriptor& InData);
        
    private:
        static bool CreatePipeline(const PipelineDescriptor& InData, uint32 InHash, WGPURenderPipeline& OutPipeline);
        
        Map<uint32, WGPURenderPipeline> cache;
        // TODO: Also track lifetime!
        // TODO: And load from file!
    };
}
