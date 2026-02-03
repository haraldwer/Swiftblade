#include "CommandList.h"

#include "Command.h"
#include "UniformBuffer.h"
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

        wgpu::TextureViewDescriptor viewDesc = target->GetView();
        wgpu::Texture tex = target->GetTexture();
        CHECK_CONTINUE(!tex);
        wgpu::TextureView view = wgpuTextureCreateView(tex, &viewDesc);
        CHECK_ASSERT(!view, "Failed to create view");
        views.push_back(view);
        
        // Create attachment using textureView
        wgpu::RenderPassColorAttachment& attachment = attachments.emplace_back();
        attachment.view = view;
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
        wgpu::TextureViewDescriptor viewDesc = InCommand.depthTarget->GetView();
        depthAttachment.view = wgpuTextureCreateView(InCommand.depthTarget->GetTexture(), &viewDesc);
        depthAttachment.depthLoadOp = InCommand.clearDepth ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load;
        depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
        depthAttachment.depthClearValue = 0.0f;
        depthAttachment.depthReadOnly = !InCommand.writeDepth;
    }

    wgpu::RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.label = wgpu::StringView("RenderPass: " + InCommand.name);
    renderPassDescriptor.nextInChain = nullptr;
    renderPassDescriptor.colorAttachmentCount = attachments.size();
    renderPassDescriptor.colorAttachments = attachments.data();
    renderPassDescriptor.depthStencilAttachment = InCommand.depthTarget ? &depthAttachment : nullptr;
    renderPassDescriptor.timestampWrites = nullptr;
    wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDescriptor);
    
    // Get material and mesh
    if (ModelResource* model = InCommand.model.Get())
    {
        // Get pipeline
        PipelineDescriptor pipelineDesc;
        pipelineDesc.label = InCommand.name;
        pipelineDesc.material = InCommand.material.Get();
        pipelineDesc.meshState = model->GetMeshState(); 
        for (auto& t : InCommand.targets)
            pipelineDesc.targetFormats.push_back(t->GetFormat());
        if (InCommand.depthTarget)
            pipelineDesc.data.depth.format = InCommand.depthTarget->GetFormat();
        pipelineDesc.data.depth.write = InCommand.writeDepth;
        pipelineDesc.data.multisampling = false;
        pipelineDesc.layout = InCommand.buffers ? 
            InCommand.buffers->GetLayout() : nullptr;
        if (wgpu::RenderPipeline* pipeline = PipelineCache::Get().GetPipeline(pipelineDesc))
        {
            renderPass.setPipeline(*pipeline);
            
            // Bind buffers
            if (InCommand.buffers)
                InCommand.buffers->Bind(renderPass);
            
            // For now, render all meshes
            // In the future: Combine mesh buffers? 
            int numMeshes = model->GetMeshCount();
            for (int i = 0; i < numMeshes; i++)
            {
                if (auto lod = model->GetMesh(i, 0))
                {
                    auto& vb = lod->vertexBuffer;
                    auto& ib = lod->indexBuffer;
                    if (vb && ib)
                    {
                        // TODO: Bind mesh-specific buffers...
                        
                        renderPass.setVertexBuffer(0, vb, 0, lod->vertexCount * lod->vertexStride);
                        renderPass.setIndexBuffer(ib, lod->indexFormat, 0, lod->indexCount * lod->indexStride);
                        renderPass.drawIndexed(lod->indexCount, 1, 0, 0, 0);
                    }
                }
            }
        }
    }
    
    if (InCommand.customFunc)
        InCommand.customFunc(renderPass);
    
    // Use renderpass
    renderPass.end();
    renderPass.release();
    
    // Free all allocated resources
    for (auto& view : views)
        view.release();
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

