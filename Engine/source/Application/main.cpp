#include "glfw3webgpu.h"  

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define WEBGPU_CPP_IMPLEMENTATION
#include "Application.h"
#include "webgpu/webgpu.hpp"

// If using Dawn
//#define WEBGPU_BACKEND_DAWN
// If using wgpu-native
//#define WEBGPU_BACKEND_WGPU
// If using emscripten
//#define WEBGPU_BACKEND_EMSCRIPTEN
// If built for web
//#if (EMSCRIPTEN)

const char* Str(const wgpu::StringView& InStr)
{
    if (InStr.data)
        return InStr.data;
    return "";
}

// Get device
wgpu::Device RequestDevice(const wgpu::Instance& InInstance, const wgpu::Adapter& InAdapter, const wgpu::DeviceDescriptor& InDeviceDescriptor)
{
    struct RequestData
    {
        wgpu::Device device;
        bool response = false;
    } data;
    wgpu::RequestDeviceCallbackInfo deviceCallbackInfo;
    deviceCallbackInfo.userdata1 = &data; 
    deviceCallbackInfo.callback = [](
        const WGPURequestDeviceStatus status, 
        const WGPUDevice device, 
        const WGPUStringView message, 
        void *pUserData1, 
        void *pUserData2)
    {
        RequestData& userData = *static_cast<RequestData*>(pUserData1);
        if (status == WGPURequestDeviceStatus_Success)
            userData.device = device;
        else
            printf("Could not get WebGPU device: %s", Str(message));
        userData.response = true;
    };
    InAdapter.requestDevice(InDeviceDescriptor, deviceCallbackInfo);
    while (!data.response)
        InInstance.processEvents();
    return data.device;
}

int main3()
{
    // Create GLFW context and window
    int initStatus = glfwInit();
    assert(initStatus == GLFW_TRUE && "Failed to initialize glfw");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1080, 1080, "WebGPU + GLFW", nullptr, nullptr);
    assert(window && "Failed to create window");
        
        
    
    // Create WGPU instance
    wgpu::InstanceDescriptor instanceDesc;
#ifdef WEBGPU_BACKEND_DAWN
    // Make sure the uncaptured error callback is called as soon as an error
    // occurs rather than at the next call to "wgpuDeviceTick".
    WGPUDawnTogglesDescriptor toggles;
        toggles.chain.next = nullptr;
        toggles.chain.sType = WGPUSType_DawnTogglesDescriptor;
        toggles.disabledToggleCount = 0;
        toggles.enabledToggleCount = 1;
        const char* toggleName = "enable_immediate_error_handling";
        toggles.enabledToggles = &toggleName;
        instanceDesc.nextInChain = &toggles.chain;
#endif // WEBGPU_BACKEND_DAWN
    wgpu::Instance instance = wgpu::createInstance(instanceDesc);
    assert(instance && "Failed to create WGPU instance");
    printf("WGPU instance: %u\n", instance);
    
    
    
    // Request Adapter and create device
    printf("Requesting adapter...\n");
    wgpu::RequestAdapterOptions adapterOptions;
    wgpu::Adapter adapter = instance.requestAdapter(adapterOptions);
    assert(adapter && "Failed to create adapter");
    printf("Got adapter: %u\n", adapter);
    
    
    
    // Get adapter limits
    wgpu::Limits adapterLimits;
    auto adapterLimitStatus = adapter.getLimits(&adapterLimits);
    if (adapterLimitStatus == WGPUStatus_Success) 
    {
        printf("Adapter limits:\n");
        printf(" - maxTextureDimension1D: %i\n", adapterLimits.maxTextureDimension1D);
        printf(" - maxTextureDimension2D: %i\n", adapterLimits.maxTextureDimension2D);
        printf(" - maxTextureDimension3D: %i\n", adapterLimits.maxTextureDimension3D);
        printf(" - maxTextureArrayLayers: %i\n", adapterLimits.maxTextureArrayLayers);
    }
    
    
    
    // Get features 
    wgpu::SupportedFeatures adapterFeatures;
    adapter.getFeatures(&adapterFeatures);
    printf("Adapter features:\n");
    for (size_t i = 0; i < adapterFeatures.featureCount; i++) 
        printf(" - %i\n", adapterFeatures.features[i]);
    
    
    
    // Get info
    wgpu::AdapterInfo info;
    adapter.getInfo(&info);
    printf("Adapter properties:\n");
    printf(" - vendorID: %i\n", info.vendorID);
    printf(" - vendorName: %s\n", Str(info.vendor));
    printf(" - architecture: %s\n", Str(info.architecture));
    printf(" - deviceID: %i\n", info.deviceID);
    printf(" - device: %s\n", Str(info.device));
    printf(" - driverDescription: %s\n", Str(info.description));
    printf(" - adapterType: %i\n", info.adapterType);
    printf(" - backendType: %i\n", info.backendType);
    
    
    
    // Get device
    printf("Requesting device...\n");
    wgpu::DeviceDescriptor deviceDesc;
    deviceDesc.label = wgpu::StringView("Default");
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = wgpu::StringView("Default queue");
    deviceDesc.deviceLostCallbackInfo.callback = [](
        WGPUDevice const* device,
        const WGPUDeviceLostReason reason,
        const WGPUStringView message, 
        void* pUserData1, 
        void* pUserData2) 
    {
        // TODO: Recreate everything!
        printf("Device lost: %i | %s\n", reason, Str(message));
        assert(false && "Device lost");
    };
    wgpu::Device device = RequestDevice(instance, adapter, deviceDesc);
    assert(device && "Failed to create device");
    printf("Got device: %u\n", device);
    
    
    
    // Inspect device
    wgpu::SupportedFeatures deviceFeatures;
    device.getFeatures(&deviceFeatures);
    printf("Device features:\n");
    for (size_t i = 0; i < deviceFeatures.featureCount; i++)
        printf(" - %i\n", deviceFeatures.features[i]);
    wgpu::Limits deviceLimits;
    if (device.getLimits(&deviceLimits) == wgpu::Status::Success) 
    {
        printf("Device limits:\n");
        printf(" - maxTextureDimension1D: %i\n", deviceLimits.maxTextureDimension1D);
        printf(" - maxTextureDimension2D: %i\n", deviceLimits.maxTextureDimension2D);
        printf(" - maxTextureDimension3D: %i\n", deviceLimits.maxTextureDimension3D);
        printf(" - maxTextureArrayLayers: %i\n", deviceLimits.maxTextureArrayLayers);
    }
    
    
    
    // Create surface
    wgpu::Surface surface = glfwCreateWindowWGPUSurface(instance, window);
    assert(surface && "Failed to create surface");
    wgpu::SurfaceCapabilities surfaceCapabilities;
    wgpu::Status surfaceCapabilityStatus = surface.getCapabilities(adapter, &surfaceCapabilities);
    assert(surfaceCapabilityStatus == wgpu::Status::Success && "Failed to get surface capabilities");
    assert(surfaceCapabilities.formatCount > 0 && "No available surface format");
    wgpu::SurfaceConfiguration surfaceConfig;
    surfaceConfig.format = surfaceCapabilities.formats[0]; 
    surfaceConfig.usage = wgpu::TextureUsage::RenderAttachment;
    surfaceConfig.width = 1080;
    surfaceConfig.height = 1080;
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.viewFormats = nullptr;
    surfaceConfig.device = device;
    surfaceConfig.presentMode = wgpu::PresentMode::Fifo;
    surfaceConfig.alphaMode = wgpu::CompositeAlphaMode::Opaque;
    surface.configure(surfaceConfig);
    
    
    
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
    wgpu::ShaderModule shaderModule = device.createShaderModule(shaderDesc);
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
    colorTarget.format = surfaceConfig.format;
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
    wgpu::RenderPipeline pipeline = device.createRenderPipeline(pipelineDesc);
    assert(pipeline && "Failed to create pipeline");
    
    
    
    // Setup the queue
    wgpu::Queue queue = device.getQueue();
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
    
    
    
    // Enter the glfw main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        
        
        // Build the queue 
        std::vector<wgpu::CommandBuffer> commands;
        // Create encoder
        wgpu::CommandEncoderDescriptor encoderDesc;
        encoderDesc.label = wgpu::StringView("Default command encoder");
        wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);
        assert(encoder && "Failed to create encoder");
        
        
        
        // Add instructions
        encoder.insertDebugMarker(wgpu::StringView("Do one thing"));
        encoder.insertDebugMarker(wgpu::StringView("Do another thing"));
        
        
        
        // Get texture and view
        wgpu::SurfaceTexture surfaceTexture;
        surface.getCurrentTexture(&surfaceTexture);
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
#ifndef __EMSCRIPTEN__
        surface.present();
#endif
#ifdef WEBGPU_BACKEND_WGPU
        if (surfaceTexture.texture)
            wgpuTextureRelease(surfaceTexture.texture);
#endif
        if (targetView)
            targetView.release();
        device.poll(false, nullptr);
    }
    
    // Cleanup!
    queue.release();
    pipeline.release();
    surface.release();
    shaderModule.release();
    device.release();
    adapter.release();
    instance.release();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}









int main()
{
    Application::App app;
    app.Init();
    app.Run();
    app.Deinit();
    return 0;
}





/*
#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif
#include "Debug/ImGui/DebugPanel.h"
#include "Utility/AssetPacker.h"

void Init();
void Deinit();
void Tick();
bool tick = true;

int main()
{
    Init();
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Tick, 0, 1); 
#else
    while (tick)
        Tick();
#endif

    Deinit();
    return 0;
}

#include "Core/Debug/Manager.h"
#include "Core/Resource/Manager.h"
#include "Database/Manager.h"
#include "Engine/Audio/Manager.h"
#include "Engine/Instance/Manager.h"
#include "Instance/Launcher.h"
#include "Rendering/Manager.h"
#include "Physics/Manager.h"
#include "Debug/Log.h"
#include "../Engine/Editor/Assets/AssetBrowser.h"

#include "Instances/MenuInstance.h"

constexpr double tickRate = 300.0; 
constexpr double fixedDelta = 1.0 / tickRate;
constexpr double maxFrameTickTime = 0.5;

struct GameData
{
    Debug::Manager debugManager;
    Debug::Log log;
    ImGui::DebugPanel imguiPanel;
    Resource::Manager resourceManager;
    AssetBrowser assetBrowser;
    Utility::AssetPacker assetPacker;
    Rendering::Manager renderer;
    Audio::Manager audio;
    Engine::Manager instanceManager;
    Physics::Persistent physics;
    Engine::Launcher launcher;
    GlobalEventManager eventManager;
    DB::Manager db = {};
    
    double logicTimeCounter = 0.0;
    Utility::Timer frameTimer;
};
GameData* g = nullptr;

void Init()
{
    Utility::File::SetWorkingDir();
    
    g = new GameData();
    g->log.Init();
    g->renderer.Init();
    g->physics.Init();
    g->debugManager.Init();
    g->assetBrowser.Init();
    g->audio.Init();
    g->launcher.Init();
    g->db.Init();
}

void Deinit()
{
    g->db.Deinit();
    g->launcher.Deinit(); 
    g->debugManager.Deinit();
    g->instanceManager.Clear();
    g->audio.Deinit();
    g->physics.Deinit();
    g->renderer.Deinit();
    g->resourceManager.Deinit();
    g->assetBrowser.Deinit();
}

void Tick()
{
    PROFILE();

    g->resourceManager.Update();
    g->db.Update();
        
    // Update instances
    g->instanceManager.Update();
        
    // Get instance
    const auto instance = g->instanceManager.Top();
    if (!instance)
    {
        tick = false;
        return;
    }
        
    // Calculate delta
    const double frameDelta = g->frameTimer.Ellapsed();
    g->frameTimer = Utility::Timer();
        
    // Update
    g->logicTimeCounter = Utility::Math::Min(g->logicTimeCounter + frameDelta, maxFrameTickTime);
    while (g->logicTimeCounter >= 0)
    {
        instance->GetRenderScene().ClearDebug();
        PROFILE_NAMED("Tick");
        g->logicTimeCounter -= fixedDelta;
        instance->Logic(fixedDelta);
        g->debugManager.Logic();
    }

    if (Rendering::Window::ShouldClose())
    {
        tick = false;
        return;
    }

    g->audio.Update();

    // Render to target texture
    instance->GetRenderScene().Clear();
    instance->Frame();
    
    // Render to screen
    g->renderer.BeginFrame();
    g->debugManager.Frame(frameDelta);
    g->renderer.EndFrame();
    PROFILE_FRAME();
}
*/