#include "CommandList.h"

#include "Command.h"
#include "../Uniforms/UniformBuffer.h"
#include "Context/Context.h"
#include "Pipeline/PipelineCache.h"
#include "Targets/RenderTarget.h"

void Rendering::CommandList::Begin(const String &InName)
{
    RN_PROFILE();
    workingName = InName;
    
    // Create encoder
    wgpu::CommandEncoderDescriptor encoderDesc;
    encoderDesc.label = wgpu::StringView("Encoder: " + workingName);
    encoder = Context::Get().CreateEncoder(encoderDesc);
    
    // Add instructions
    encoder.insertDebugMarker(wgpu::StringView("Begin: " + workingName));
}

void Rendering::CommandList::Add(const Command& InCommand)
{
    RN_PROFILE();
    CHECK_ASSERT(InCommand.targets.empty(), "No targets for command");
    
    // Create attachments and views
    Vector<wgpu::RenderPassColorAttachment> attachments;
    Vector<wgpu::TextureView> views;
    attachments.reserve(InCommand.targets.size());
    views.reserve(InCommand.targets.size());
    for (auto& target : InCommand.targets)
    {
        CHECK_ASSERT(!target, "Invalid target");
        CHECK_ASSERT(!target->view, "Invalid view");
        views.push_back(target->view);
        
        // Create attachment using textureView
        wgpu::RenderPassColorAttachment& attachment = attachments.emplace_back();
        attachment.view = target->view;
        attachment.resolveTarget = nullptr;
        attachment.loadOp = InCommand.clear ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load;
        attachment.storeOp = wgpu::StoreOp::Store;
        attachment.clearValue = { 
            InCommand.clearColor.x, 
            InCommand.clearColor.y, 
            InCommand.clearColor.z, 
            InCommand.clearColor.w 
        };
    #ifndef WEBGPU_BACKEND_WGPU
        attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    #endif
    }
    
    wgpu::RenderPassDepthStencilAttachment depthAttachment;
    if (InCommand.depthTarget)
    {
        CHECK_ASSERT(!InCommand.depthTarget->view, "Invalid depth view");
        depthAttachment.view = InCommand.depthTarget->view;
        depthAttachment.depthLoadOp = InCommand.clearDepth ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load;
        depthAttachment.depthStoreOp = InCommand.writeDepth ? wgpu::StoreOp::Store : wgpu::StoreOp::Discard;
        depthAttachment.depthClearValue = 1.0f;
        depthAttachment.depthReadOnly = !InCommand.writeDepth;
        // TODO: If depthReadOnly, then depthLoadOp cannot be LoadOp::Clear
    }

    String name = "RenderPass: " + InCommand.name;
    wgpu::RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.label = wgpu::StringView(name);
    renderPassDescriptor.nextInChain = nullptr;
    renderPassDescriptor.colorAttachmentCount = attachments.size();
    renderPassDescriptor.colorAttachments = attachments.data();
    renderPassDescriptor.depthStencilAttachment = InCommand.depthTarget ? &depthAttachment : nullptr;
    renderPassDescriptor.timestampWrites = nullptr;
    wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDescriptor);
    
    InCommand.model.Identifier().IsValid() ? 
        RenderModel(InCommand, renderPass) : 
        RenderFullscreen(InCommand, renderPass);
    
    if (InCommand.customFunc)
        InCommand.customFunc(renderPass);
    
    // Use renderpass
    renderPass.end();
    renderPass.release();
}

void Rendering::CommandList::RenderModel(const Command &InCommand, const wgpu::RenderPassEncoder& renderPass) 
{
    // Get material and mesh
    ModelResource* model = InCommand.model.Get();
    CHECK_RETURN(!model);
    const MeshState* meshState = model->GetMeshState();
    CHECK_RETURN(!meshState);
    
    // Get pipeline
    PipelineDescriptor pipelineDesc;
    pipelineDesc.label = InCommand.name;
    pipelineDesc.material = InCommand.material.Get();
    pipelineDesc.meshState = model->GetMeshState();
    pipelineDesc.data.multisampling = InCommand.targets.front()->descriptor.multisample;
    for (auto& t : InCommand.targets)
    {
        pipelineDesc.targetFormats.push_back(t->descriptor.format);
        CHECK_ASSERT(pipelineDesc.data.multisampling != t->descriptor.multisample, "Multisample state must match");
    }
    if (InCommand.depthTarget)
        pipelineDesc.data.depth.format = InCommand.depthTarget->descriptor.format;
    pipelineDesc.data.depth.write = InCommand.writeDepth;
    
    pipelineDesc.layout = InCommand.buffers ? 
        InCommand.buffers->GetLayout() : nullptr;
    
    wgpu::RenderPipeline* pipeline = PipelineCache::Get().GetPipeline(pipelineDesc);
    CHECK_RETURN(!pipeline);
    
    renderPass.setPipeline(*pipeline);
        
    // Bind buffers
    if (InCommand.buffers)
        InCommand.buffers->Bind(renderPass);
        
    // For now, render all meshes
    // In the future: Combine mesh buffers? 
    int numMeshes = model->GetMeshCount();
    for (int i = 0; i < numMeshes; i++)
    {
        auto lod = model->GetMesh(i, 0);
        CHECK_CONTINUE(!lod);
        auto& vb = lod->vertexBuffer;
        auto& ib = lod->indexBuffer;
        CHECK_RETURN(!vb || !ib);
        
        // TODO: Bind mesh-specific buffers...            
        renderPass.setVertexBuffer(0, vb, 0, lod->vertexCount * lod->vertexStride);
        renderPass.setIndexBuffer(ib, lod->indexFormat, 0, lod->indexCount * lod->indexStride);
        renderPass.drawIndexed(lod->indexCount, 1, 0, 0, 0);
    }
}

void Rendering::CommandList::RenderFullscreen(const Command &InCommand, const wgpu::RenderPassEncoder& renderPass) 
{
    // Fullscreen pass
    // Get pipeline
    PipelineDescriptor pipelineDesc;
    pipelineDesc.label = InCommand.name;
    pipelineDesc.material = InCommand.material.Get();
    pipelineDesc.meshState = nullptr; 
    for (auto& t : InCommand.targets)
        pipelineDesc.targetFormats.push_back(t->descriptor.format);
    if (InCommand.depthTarget)
        pipelineDesc.data.depth.format = InCommand.depthTarget->descriptor.format;
    pipelineDesc.data.depth.write = InCommand.writeDepth;
    pipelineDesc.data.multisampling = false;
    pipelineDesc.layout = InCommand.buffers ? 
        InCommand.buffers->GetLayout() : nullptr;
    
    wgpu::RenderPipeline* pipeline = PipelineCache::Get().GetPipeline(pipelineDesc);
    CHECK_RETURN(!pipeline);
    
    if (InCommand.buffers)
        InCommand.buffers->Bind(renderPass);
    
    renderPass.setPipeline(*pipeline);
    renderPass.draw(3, 1, 0, 0);
}

void Rendering::CommandList::End()
{
    RN_PROFILE();
    encoder.insertDebugMarker(wgpu::StringView("End: " + workingName));
    
    wgpu::CommandBufferDescriptor commandBufferDesc;
    commandBufferDesc.label = wgpu::StringView("Command: " + workingName);
    commands.emplace_back() = encoder.finish(commandBufferDesc);
    encoder.release();
    encoder = {};
    workingName = "";
}

void Rendering::CommandList::Submit()
{
    RN_PROFILE();
    Context::Get().Submit(commands);
    for (auto& cmd : commands)
        cmd.release();
    commands.clear();
}

