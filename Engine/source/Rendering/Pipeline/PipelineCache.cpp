#include "PipelineCache.h"

#include "UniformBuffer.h"
#include "Context/Context.h"
#include "Resources/Material.h"
#include "Resources/Model.h"

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

uint32 GetHash(const Rendering::PipelineDescriptor& InData)
{
    struct FullHash
    {
        uint32 materialHash = 0; 
        uint32 meshHash = 0;
        uint32 targetHash = 0; 
        uint32 staticHash = 0;
        uint32 layoutHash = 0;
    } fullHash;
    fullHash.materialHash = InData.material->Hash();
    fullHash.meshHash = InData.meshState->hash;
    fullHash.targetHash = Utility::Hash(InData.targetFormats);
    fullHash.staticHash = Utility::Hash(InData.data);
    fullHash.layoutHash = InData.layout ? InData.layout->hash : 0;
    return Utility::Hash(fullHash);
}

wgpu::RenderPipeline* Rendering::PipelineCache::GetPipeline(const PipelineDescriptor& InData)
{
    RN_PROFILE();
    
    CHECK_RETURN(!InData.material, nullptr);
    CHECK_RETURN(!InData.meshState, nullptr);
    CHECK_RETURN(InData.targetFormats.empty(), nullptr);
    
    uint32 hash = GetHash(InData);
    auto find = cache.find(hash);
    if (find != cache.end())
        return &find->second;
    
    wgpu::RenderPipeline pipeline;
    if (!CreatePipeline(InData, hash, pipeline))
        return nullptr;
    auto& pipelineRef = cache[hash];
    pipelineRef = pipeline;
    return &pipelineRef;
}

bool Rendering::PipelineCache::CreatePipeline(const PipelineDescriptor& InData, uint32 InHash, wgpu::RenderPipeline& OutPipeline)
{
    RN_PROFILE();
    
    CHECK_ASSERT(!InData.material, "Invalid material")
    CHECK_ASSERT(!InData.meshState, "Invalid meshState");
    CHECK_ASSERT(InData.targetFormats.empty(), "No targets");
    
    ShaderResource* shaderResource = InData.material->GetShader().Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to get shader", false);
    
    wgpu::VertexState vertexState{};
    wgpu::PrimitiveState primitiveState{};
    wgpu::FragmentState fragmentState{};
    wgpu::DepthStencilState depthStencilState{};
    wgpu::MultisampleState multisampleState{};
    
    // Vertex
    vertexState.entryPoint = wgpu::StringView("vs_main");
    vertexState.module = shaderResource->shader;
    vertexState.bufferCount = static_cast<uint32_t>(InData.meshState->vertexLayouts.size());
    vertexState.buffers = InData.meshState->vertexLayouts.data();
    vertexState.constantCount = 0;
    vertexState.constants = nullptr;

    // Primitive
    primitiveState = InData.meshState->primitiveState;
    
    // Fragment
    fragmentState.entryPoint = wgpu::StringView("fs_main");
    fragmentState.module = shaderResource->shader;
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    wgpu::BlendState blendState{};
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    blendState.alpha.srcFactor = wgpu::BlendFactor::One;
    blendState.alpha.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.alpha.operation = wgpu::BlendOperation::Add;
    Vector<wgpu::ColorTargetState> targetStates;
    for (auto& target : InData.targetFormats)
    {
        wgpu::ColorTargetState& colorTarget = targetStates.emplace_back();
        colorTarget.format = target;
        colorTarget.blend = &blendState;
        colorTarget.writeMask = wgpu::ColorWriteMask::All;
    }
    fragmentState.targetCount = static_cast<uint32_t>(targetStates.size());
    fragmentState.targets = targetStates.data();
    
    // Depth/stencil
    if (InData.data.depth.format != wgpu::TextureFormat::Undefined)
    {
        depthStencilState.format = InData.data.depth.format;
        depthStencilState.depthCompare = wgpu::CompareFunction::Less;
        depthStencilState.depthWriteEnabled = InData.data.depth.write ? wgpu::OptionalBool::True : wgpu::OptionalBool::False; // set explicitly
        depthStencilState.stencilReadMask = 0xFFFFFFFFu;
        depthStencilState.stencilWriteMask = 0xFFFFFFFFu;
    }
    
    // Multisample
    multisampleState.count = 1;
    multisampleState.mask = ~0u;
    multisampleState.alphaToCoverageEnabled = false;
    
    // Create final descriptor
    String label = InData.label + "_" + Utility::ToStr(InHash);
    wgpu::RenderPipelineDescriptor desc;
    desc.label = wgpu::StringView(label);
    desc.vertex = vertexState;
    desc.primitive = primitiveState;
    desc.fragment = &fragmentState;
    desc.depthStencil = InData.data.depth.format == wgpu::TextureFormat::Undefined ? nullptr : &depthStencilState;
    desc.multisample = multisampleState;
    desc.layout = InData.layout ? InData.layout->layout : nullptr;
    
    OutPipeline = Context::Get().CreatePipeline(desc);
    LOG("Pipeline created: ", label);
    return true;
}

