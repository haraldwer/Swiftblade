#include "CommandList.h"

#include "Command.h"
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

void Rendering::CommandList::Add(const Command &InCommand)
{
    RN_PROFILE();
    CHECK_ASSERT(InCommand.targets.empty(), "No targets for command");
    
    // Get material and mesh
    auto material = InCommand.material.Get();
    CHECK_RETURN_LOG(!material, "Failed to get material")
    auto model = InCommand.model.Get();
    CHECK_RETURN_LOG(!material, "Failed to get material")
    
    wgpu::PrimitiveState meshState; 
    model->GetMeshState(meshState);
    
    // TODO: Who decides blending? Material?
    // Configure color blending equation
    wgpu::BlendState blendState;
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    // [...] Configure alpha blending equation
    
    // Configure targetStates
    Vector<wgpu::ColorTargetState> targetStates;
    for (auto& target : InCommand.targets)
    {
        wgpu::ColorTargetState& colorTarget = targetStates.emplace_back();
        colorTarget.format = target->GetFormat();
        colorTarget.blend = &blendState;
        colorTarget.writeMask = wgpu::ColorWriteMask::All;
    }
    
    // Get pipeline
    wgpu::RenderPipeline pipeline = PipelineCache::Get().GetPipeline(*material, targetStates, meshState);
    CHECK_RETURN_LOG(!pipeline, "Failed to get pipeline")
    
    // Create attachments and views
    Vector<wgpu::RenderPassColorAttachment> attachments;
    Vector<wgpu::TextureView> views;
    attachments.reserve(InCommand.targets.size());
    views.reserve(InCommand.targets.size());
    for (auto& target : InCommand.targets)
    {
        CHECK_ASSERT(!target, "Invalid target");

        wgpu::TextureViewDescriptor viewDesc = target->GetView();
        wgpu::TextureView view = wgpuTextureCreateView(target->GetTexture(), &viewDesc);
        CHECK_ASSERT(!view, "Failed to create view");
        views.push_back(view);
        
        // Create attachment using textureView
        wgpu::RenderPassColorAttachment& attachment = attachments.emplace_back();
        attachment.view = view;
        attachment.resolveTarget = nullptr;
        attachment.loadOp = wgpu::LoadOp::Clear;
        attachment.storeOp = wgpu::StoreOp::Store;
        attachment.clearValue = { 0.2f, 0.8f, 0.34f, 1.0f };
    #ifndef WEBGPU_BACKEND_WGPU
        attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    #endif
    }
    
    wgpu::RenderPassDepthStencilAttachment depthAttachment;
    if (InCommand.depthTarget)
    {
        // Setup depth attachment
    }

    
    
    
    
    /*
    {
        wgpu::BufferDescriptor inBufferDesc;
        inBufferDesc.label = wgpu::StringView("Input Buffer");
        inBufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc;
        inBufferDesc.size = 16;
        inBufferDesc.mappedAtCreation = false;
        wgpu::Buffer buffer1 = Context::Get().CreateBuffer(inBufferDesc);
        
        // Write to buffer and upload to gpu (This is done before encode command)
        std::vector<uint8_t> numbers(16);
        for (uint8_t i = 0; i < 16; ++i) 
            numbers[i] = i;
        queue.writeBuffer(buffer1, 0, numbers.data(), numbers.size());
        
        // Copy between buffers on gpu
        wgpu::BufferDescriptor outBufferDesc;
        outBufferDesc.label = wgpu::StringView("Output Buffer");
        outBufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
        outBufferDesc.size = 16;
        outBufferDesc.mappedAtCreation = false;
        wgpu::Buffer buffer2 = Context::Get().CreateBuffer(outBufferDesc);
        encoder.copyBufferToBuffer(buffer1, 0, buffer2, 0, 16);
        
        // Read from GPU buffer
        struct BufferMapCallback
        {
            wgpu::Buffer buffer;
            
        } callbackData;
        callbackData.buffer = buffer2;
        wgpu::BufferMapCallbackInfo bufferCallbackInfo;
        bufferCallbackInfo.userdata1 = &callbackData; // Dont forget to MANAGE LIFETIME
        bufferCallbackInfo.callback = [](WGPUMapAsyncStatus InStatus, WGPUStringView InMessage, void *InUserData1, void *InUserData2)
        {
            LOG("Buffer mapped with status ", InStatus, " | ", ToStr(InMessage));
            BufferMapCallback* callbackData = static_cast<BufferMapCallback*>(InUserData1);
            uint8_t* bufferData = (uint8_t*)callbackData->buffer.getConstMappedRange(0, 16); // Get mapped memory
            // Use memory
            std::cout << "bufferData = [";
            for (int i = 0; i < 16; ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << (int)bufferData[i];
            }
            std::cout << "]" << std::endl;
            // Unmap
            callbackData->buffer.unmap();
        };
        wgpuBufferMapAsync(buffer2, wgpu::MapMode::Read, 0, 16, bufferCallbackInfo);
        
        buffer1.release();
        buffer2.release();
    }
    */
    
    
    // Vertex buffer data
    // There are 2 floats per vertex, one for x and one for y.
    // But in the end this is just a bunch of floats to the eyes of the GPU,
    // the *layout* will tell how to interpret this.
    std::vector<float> vertexData = {
        // x0, y0
        -0.5, -0.5,

        // x1, y1
        +0.5, -0.5,

        // x2, y2
        +0.0, +0.5
    };
    
    // We will declare vertexCount as a member of the Application class
    uint32 vertexCount = static_cast<uint32_t>(vertexData.size() / 2);
    
    // Create vertex buffer
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = vertexData.size() * sizeof(float);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex; // Vertex usage here!
    bufferDesc.mappedAtCreation = false;
    wgpu::Buffer vertexBuffer = Context::Get().CreateBuffer(bufferDesc);

    // Upload geometry data to the buffer
    queue.writeBuffer(vertexBuffer, 0, vertexData.data(), bufferDesc.size);
    
    
    
    
    
    
    wgpu::RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.label = wgpu::StringView("RenderPass: " + workingName);
    renderPassDescriptor.nextInChain = nullptr;
    renderPassDescriptor.colorAttachmentCount = attachments.size();
    renderPassDescriptor.colorAttachments = attachments.data();
    renderPassDescriptor.depthStencilAttachment = InCommand.depthTarget ? &depthAttachment : nullptr;
    renderPassDescriptor.timestampWrites = nullptr;
    wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDescriptor);
    
    // Set pipeline
    renderPass.setPipeline(pipeline);
    // Draw 1 instance of a 3-vertices shape
    renderPass.draw(3, 1, 0, 0);
    // Or possibly, draw a number of meshes...
    
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
