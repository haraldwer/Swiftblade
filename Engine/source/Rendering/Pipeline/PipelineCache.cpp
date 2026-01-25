#include "PipelineCache.h"

#include "Context/Context.h"
#include "Resources/Material.h"

void Rendering::PipelineCache::Init()
{
    RN_PROFILE();
    CHECK_ASSERT(!cache.empty(), "Cache already initialized");
    // TODO: Load cache?
}

void Rendering::PipelineCache::Deinit()
{
    RN_PROFILE();
    for (auto& pipeline : cache)
        pipeline.second.release();
    cache.clear();
}

wgpu::RenderPipeline Rendering::PipelineCache::GetPipeline(const MaterialResource& InMaterial, const Vector<wgpu::ColorTargetState>& InTargetStates, const wgpu::PrimitiveState& InMeshState)
{
    RN_PROFILE();
    union Data
    {
        uint64 key;
        struct
        {
            uint32 materialHash; 
            uint16 targetHash; 
            uint16 layoutHash;
        };
    } hash;
    
    hash.materialHash = InMaterial.id.Hash();
    hash.targetHash = 0; // TODO:
    hash.layoutHash = 0; // TODO:
    
    auto find = cache.find(hash.key);
    if (find != cache.end())
        return find->second;
    
    auto pipeline = CreatePipeline(InMaterial, InTargetStates, InMeshState);
    if (pipeline)
        cache[hash.key] = pipeline;
    return pipeline;
}

wgpu::RenderPipeline Rendering::PipelineCache::CreatePipeline(const MaterialResource &InMaterial, const Vector<wgpu::ColorTargetState> &InTargetStates, const wgpu::PrimitiveState &InMeshState)
{
    RN_PROFILE();
    
    // Get shader
    ShaderResource* shaderResource = InMaterial.GetShader().Get();
    CHECK_RETURN(!shaderResource, {});
    
    // Create pipeline
    wgpu::RenderPipelineDescriptor pipelineDesc;
    pipelineDesc.label = wgpu::StringView("Render pipeline");
    
    // Describe vertex pipeline state
    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers = nullptr;
    pipelineDesc.vertex.module = shaderResource->shader;
    pipelineDesc.vertex.entryPoint = wgpu::StringView("vs_main");
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;
    
    // Describe fragment pipeline state
    wgpu::FragmentState fragmentState;
    fragmentState.module = shaderResource->shader;
    fragmentState.entryPoint = wgpu::StringView("fs_main");
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    
    // Describe primitive pipeline state
    pipelineDesc.primitive = InMeshState;
    
    // Set which targets are being rendered to
    fragmentState.targetCount = InTargetStates.size();
    fragmentState.targets = InTargetStates.data();
    pipelineDesc.fragment = &fragmentState;

    
    
    
    // Describe stencil/depth pipeline state
    pipelineDesc.depthStencil = nullptr;
    
    // Describe multi-sampling state
    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;
    
    // Describe pipeline layout (buffers and textures)
    pipelineDesc.layout = nullptr;
    
    
    
    
    auto pipeline = Context::Get().CreatePipeline(pipelineDesc);
    CHECK_ASSERT(!pipeline, "Failed to create pipeline");
    return pipeline;
}
