#include "Renderer.h"

const char* Str(const wgpu::StringView& InStr)
{
    if (InStr.data)
        return InStr.data;
    return "";
}

void Application::Renderer::Init()
{
    context.Init({});
    window.Open(context, {});
    pacer = Rendering::FramePacer();
    
    
    
    // Create shader
    wgpu::StringView shaderSource = wgpu::StringView(R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
    var p = vec2f(0.0, 0.0);
    if (in_vertex_index == 0u) {
        p = vec2f(-0.5, -0.5);
    } else if (in_vertex_index == 1u) {
        p = vec2f(0.5, -0.5);
    } else {
        p = vec2f(0.0, 0.5);
    }
    return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(0.0, 0.4, 1.0, 1.0);
})");
    wgpu::ShaderModuleDescriptor shaderDesc;
    shaderDesc.label = wgpu::StringView("Default shader");
    wgpu::ShaderSourceWGSL shaderCodeSource;
    shaderCodeSource.chain.next = nullptr;
    shaderCodeSource.chain.sType = WGPUSType_ShaderSourceWGSL;
    shaderDesc.nextInChain = &shaderCodeSource.chain;
    shaderCodeSource.code = shaderSource;
    shaderModule = context.device.createShaderModule(shaderDesc);
    assert(shaderModule && "Failed to create shader module");
    
    
    
    // Create pipeline
    wgpu::RenderPipelineDescriptor pipelineDesc;
    pipelineDesc.label = wgpu::StringView("Render pipeline");
    // Describe vertex pipeline state
    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers = nullptr;
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = wgpu::StringView("vs_main");
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;
    // Describe primitive pipeline state
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;
    // Describe fragment pipeline state
    wgpu::FragmentState fragmentState;
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = wgpu::StringView("fs_main");
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    // Configure blending
    wgpu::BlendState blendState;
    // Configure color blending equation
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    // [...] Configure alpha blending equation
    wgpu::ColorTargetState colorTarget;
    colorTarget.format = wgpu::TextureFormat::RGBA8Uint;// surfaceConfig.format;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = wgpu::ColorWriteMask::All;
    // Set which targets are being rendered to
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;
    pipelineDesc.fragment = &fragmentState;
    // Describe stencil/depth pipeline state
    pipelineDesc.depthStencil = nullptr;
    // Describe multi-sampling state
    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;
    // Describe pipeline layout (buffers and textures)
    pipelineDesc.layout = nullptr;
    pipeline = context.device.createRenderPipeline(pipelineDesc);
    assert(pipeline && "Failed to create pipeline");
    
    
    
    // Setup the queue
    queue = context.device.getQueue();
    assert(queue && "Failed to create queue");
    wgpu::QueueWorkDoneCallbackInfo queueDoneCallbackInfo;
    queueDoneCallbackInfo.callback = [](
        const WGPUQueueWorkDoneStatus status, 
        void* pUserData1,
        void* pUserData2) 
    {
        assert(WGPUQueueWorkDoneStatus_Success && "Queue failed to submit work");
    };
    queue.onSubmittedWorkDone(queueDoneCallbackInfo);
}

void Application::Renderer::Deinit()
{
    queue.release();
    pipeline.release();
    shaderModule.release();
    
    window.Close();
    context.Deinit();
}

int Application::Renderer::Frame(bool& InRun)
{
    // Build the queue 
    std::vector<wgpu::CommandBuffer> commands;
    // Create encoder
    wgpu::CommandEncoderDescriptor encoderDesc;
    encoderDesc.label = wgpu::StringView("Default command encoder");
    wgpu::CommandEncoder encoder = context.device.createCommandEncoder(encoderDesc);
    assert(encoder && "Failed to create encoder");
    
    
    
    // Add instructions
    encoder.insertDebugMarker(wgpu::StringView("Do one thing"));
    encoder.insertDebugMarker(wgpu::StringView("Do another thing"));
    
    
    
    // Get texture and view
    wgpu::SurfaceTexture surfaceTexture;
    window.surface.getCurrentTexture(&surfaceTexture);
    assert(surfaceTexture.status <= wgpu::SurfaceGetCurrentTextureStatus::Timeout && "Failed to get surface texture");
    wgpu::TextureView targetView;
    // If valid texture -> Begin rendering
    if (surfaceTexture.texture)
    {
        // Create textureView (describing how texture is used)
        wgpu::TextureViewDescriptor textureViewDesc;
        textureViewDesc.nextInChain = nullptr;
        textureViewDesc.label = wgpu::StringView("Surface texture view");
        textureViewDesc.format = wgpuTextureGetFormat(surfaceTexture.texture);
        textureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
        textureViewDesc.baseMipLevel = 0;
        textureViewDesc.mipLevelCount = 1;
        textureViewDesc.baseArrayLayer = 0;
        textureViewDesc.arrayLayerCount = 1;
        textureViewDesc.aspect = wgpu::TextureAspect::All;
        targetView = wgpuTextureCreateView(surfaceTexture.texture, &textureViewDesc);

        
        
        // Create attachment using textureView
        wgpu::RenderPassColorAttachment renderPassColorAttachment;
        renderPassColorAttachment.view = targetView;
        renderPassColorAttachment.resolveTarget = nullptr;
        renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
        renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
        renderPassColorAttachment.clearValue = { 0.2f, 0.8f, 0.34f, 1.0f };
#ifndef WEBGPU_BACKEND_WGPU
        renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif
        wgpu::RenderPassDescriptor renderPassDescriptor;
        renderPassDescriptor.nextInChain = nullptr;
        renderPassDescriptor.colorAttachmentCount = 1;
        renderPassDescriptor.colorAttachments = &renderPassColorAttachment;
        renderPassDescriptor.depthStencilAttachment = nullptr;
        renderPassDescriptor.timestampWrites = nullptr;
        wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDescriptor);
        
        
        
        // Set pipeline
        renderPass.setPipeline(pipeline);
        // Draw 1 instance of a 3-vertices shape
        renderPass.draw(3, 1, 0, 0);
        
        
        
        // Use renderpass
        renderPass.end();
        renderPass.release();
    }
    
    
    
    // Build command
    wgpu::CommandBufferDescriptor commandBufferDesc;
    commandBufferDesc.label = wgpu::StringView("Default command buffer");
    commands.emplace_back() = encoder.finish();
    encoder.release();
    
    
    
    // Submit the queue
    queue.submit(commands.size(), commands.data());
    for (auto& cmd : commands)
        cmd.release();
    commands.clear();
    
    
    
    // Step 16: Present and wait
    window.Present();
#ifdef WEBGPU_BACKEND_WGPU
    if (surfaceTexture.texture)
        wgpuTextureRelease(surfaceTexture.texture);
#endif
    if (targetView)
        targetView.release();
    context.device.poll(false, nullptr);
    window.Frame(InRun);
    
    return pacer.Pace();
}
