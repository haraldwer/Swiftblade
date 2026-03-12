#include "PipelineCache.h"

#include "../Uniforms/UniformBuffer.h"
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
        wgpuRenderPipelineRelease(pipeline.second);
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
    fullHash.meshHash = InData.meshState ? InData.meshState->hash : 0;
    fullHash.targetHash = Utility::Hash(InData.targetFormats);
    fullHash.staticHash = Utility::Hash(InData.data);
    fullHash.layoutHash = InData.layout ? InData.layout->hash : 0;
    return Utility::Hash(fullHash);
}

WGPURenderPipeline* Rendering::PipelineCache::GetPipeline(const PipelineDescriptor& InData)
{
    RN_PROFILE();
    
    CHECK_RETURN(!InData.material, nullptr);
    CHECK_RETURN(InData.targetFormats.empty(), nullptr);
    
    uint32 hash = GetHash(InData);
    auto find = cache.find(hash);
    if (find != cache.end())
        return &find->second;
    
    WGPURenderPipeline pipeline;
    if (!CreatePipeline(InData, hash, pipeline))
        return nullptr;
    auto& pipelineRef = cache[hash];
    pipelineRef = pipeline;
    return &pipelineRef;
}

bool Rendering::PipelineCache::CreatePipeline(const PipelineDescriptor& InData, uint32 InHash, WGPURenderPipeline& OutPipeline)
{
    RN_PROFILE();
    
    CHECK_ASSERT(!InData.material, "Invalid material")
    CHECK_ASSERT(InData.targetFormats.empty(), "No targets");
    
    ShaderResource* shaderResource = InData.material->GetShader().Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to get shader", false);
    
    WGPUVertexState vertexState{};
    WGPUPrimitiveState primitiveState{};
    WGPUFragmentState fragmentState{};
    WGPUDepthStencilState depthStencilState{};
    WGPUMultisampleState multisampleState{};
    
    // Vertex
    vertexState.nextInChain = nullptr;
    vertexState.bufferCount = 0;
    vertexState.buffers = nullptr;
    vertexState.constantCount = 0;
    vertexState.constants = nullptr;
    vertexState.entryPoint = WGPUStringView("vs_main");
    vertexState.module = shaderResource->shader;
    
    // Primitive
    primitiveState.nextInChain = nullptr;
    primitiveState.cullMode = WGPUCullMode_Back;
    primitiveState.frontFace = WGPUFrontFace_CCW;
    primitiveState.stripIndexFormat = WGPUIndexFormat_Uint32;
    primitiveState.topology = WGPUPrimitiveTopology_LineList;
    primitiveState.unclippedDepth = false;
    
    if (InData.meshState)
    {
        vertexState.bufferCount = static_cast<uint32_t>(InData.meshState->vertexLayouts.size());
        vertexState.buffers = InData.meshState->vertexLayouts.data();
        primitiveState = InData.meshState->primitiveState;
    }
    
    // Fragment
    fragmentState.nextInChain = nullptr;
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    fragmentState.targetCount = 0;
    fragmentState.targets = nullptr;
    fragmentState.entryPoint = WGPUStringView("fs_main");
    fragmentState.module = shaderResource->shader;
    
    WGPUBlendState blendState{};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;
    blendState.alpha.srcFactor = WGPUBlendFactor_One;
    blendState.alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.alpha.operation = WGPUBlendOperation_Add;
    
    Vector<WGPUColorTargetState> targetStates;
    for (auto& target : InData.targetFormats)
    {
        WGPUColorTargetState& colorTarget = targetStates.emplace_back();
        colorTarget.format = target;
        colorTarget.blend = &blendState;
        colorTarget.writeMask = WGPUColorWriteMask_All;
    }
    fragmentState.targetCount = static_cast<uint32_t>(targetStates.size());
    fragmentState.targets = targetStates.data();
    
    // Depth/stencil
    if (InData.data.depth.format != WGPUTextureFormat_Undefined)
    {
        depthStencilState.format = InData.data.depth.format;
        depthStencilState.depthCompare = WGPUCompareFunction_Less;
        depthStencilState.depthWriteEnabled = InData.data.depth.write ? WGPUOptionalBool_True : WGPUOptionalBool_False;
        depthStencilState.stencilReadMask = 0;
        depthStencilState.stencilWriteMask = 0;
    }
    
    // Multisample
    multisampleState.count = Utility::Math::Clamp(InData.data.multisampling, 1, 8);
    multisampleState.mask = ~0u;
    multisampleState.alphaToCoverageEnabled = false;
    
    // Create final descriptor
    String label = InData.label + "_" + Utility::ToStr(InHash);
    WGPURenderPipelineDescriptor desc;
    desc.label = WGPUStringView(label.c_str());
    desc.vertex = vertexState;
    desc.primitive = primitiveState;
    desc.fragment = &fragmentState;
    desc.depthStencil = InData.data.depth.format == WGPUTextureFormat_Undefined ? nullptr : &depthStencilState;
    desc.multisample = multisampleState;
    desc.layout = InData.layout ? InData.layout->layout : nullptr;
    
    OutPipeline = Context::Get().CreatePipeline(desc);
    CHECK_ASSERT(!OutPipeline, "Failed to create pipeline");
    LOG("Pipeline created: ", label);
    return true;
}

