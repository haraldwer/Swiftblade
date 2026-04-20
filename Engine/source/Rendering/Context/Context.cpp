#include "Context.h"


#include "webgpu/webgpu.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include "GLFW/glfw3.h"
#else
#include "glfw3webgpu.h"
#include "webgpu/wgpu.h"
#endif

#include "Window/Window.h"

void Rendering::Context::BeginInit(const ContextConfig& InConfig)
{
    RN_PROFILE();
    
    config = InConfig;
    InitGLFW();
    InitInstance();
}

void Rendering::Context::EndInit(const Window &InWindow)
{
    InitAdapter(InWindow);
    InitDevice();
    InitQueue();
    
    ConfigureWindowSurface(InWindow);
}

void Rendering::Context::InitGLFW() 
{
    RN_PROFILE();
    
    // Create GLFW context and window
    int initStatus = glfwInit();
    CHECK_ASSERT(initStatus != GLFW_TRUE, "Failed to initialize glfw");
}

void Rendering::Context::InitInstance() 
{
    RN_PROFILE();
    WGPUInstanceDescriptor instanceDesc = {};
#if false
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
#endif
    
    LOG("Creating wgpu instance...")
    instance = wgpuCreateInstance(&instanceDesc);
    CHECK_ASSERT(!instance, "Failed to create WGPU instance");
}

void Rendering::Context::InitAdapter(const Window &InWindow)
{
    RN_PROFILE();
    
    // Request Adapter and create device
    LOG("Requesting adapter...");
    WGPURequestAdapterOptions adapterOptions = {};
    adapterOptions.powerPreference = WGPUPowerPreference_HighPerformance;
#ifdef EMSCRIPTEN
    adapterOptions.backendType = WGPUBackendType_WebGPU;
    adapterOptions.compatibleSurface = InWindow.surface;
#endif
    WGPURequestAdapterCallbackInfo adapterCallbackInfo = {};
    adapterCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    adapterCallbackInfo.callback = [](
        WGPURequestAdapterStatus status, 
        WGPUAdapter adapter, 
        WGPUStringView message, 
        void* data1, 
        void* data2)
    {
        auto c = static_cast<Context*>(data1);
        c->adapter = adapter;
        auto response = static_cast<bool*>(data2);
        *response = true;
    };
    bool response = false;
    adapterCallbackInfo.userdata1 = this;
    adapterCallbackInfo.userdata2 = &response;
    wgpuInstanceRequestAdapter(instance, &adapterOptions, adapterCallbackInfo);
    while (!response)
    {
#ifdef EMSCRIPTEN
        emscripten_sleep(10);
#endif
        wgpuInstanceProcessEvents(instance);
    }
    CHECK_ASSERT(!adapter, "Failed to create adapter");
    
    // Get adapter limits
    adapterLimits = {};
    WGPUStatus adapterLimitStatus = wgpuAdapterGetLimits(adapter, &adapterLimits);
    if (adapterLimitStatus == WGPUStatus_Success) 
    {
        LOG("Adapter limits:");
        LOG(" - maxTextureDimension1D: ", adapterLimits.maxTextureDimension1D);
        LOG(" - maxTextureDimension2D: ", adapterLimits.maxTextureDimension2D);
        LOG(" - maxTextureDimension3D: ", adapterLimits.maxTextureDimension3D);
        LOG(" - maxTextureArrayLayers: ", adapterLimits.maxTextureArrayLayers);
        LOG(" - maxVertexAttributes: ", adapterLimits.maxVertexAttributes);
    }
    
    // Get features 
    adapterFeatures = {};
    wgpuAdapterGetFeatures(adapter, &adapterFeatures);
    LOG("Adapter features:");
    for (size_t i = 0; i < adapterFeatures.featureCount; i++) 
        LOG(" - ", adapterFeatures.features[i]);
    
    // Get info
    adapterInfo = {};
    wgpuAdapterGetInfo(adapter, &adapterInfo);
    LOG("Adapter properties:");
    LOG(" - vendorID: ", adapterInfo.vendorID);
    LOG(" - vendorName: ", ToStr(adapterInfo.vendor));
    LOG(" - architecture: ", ToStr(adapterInfo.architecture));
    LOG(" - deviceID: ", adapterInfo.deviceID);
    LOG(" - device: ", ToStr(adapterInfo.device));
    LOG(" - driverDescription: ", ToStr(adapterInfo.description));
    LOG(" - adapterType: ", adapterInfo.adapterType);
    LOG(" - backendType: ", adapterInfo.backendType);
}

void Rendering::Context::InitDevice() 
{
    RN_PROFILE();
    // Get device
    LOG("Requesting device...");
    WGPUDeviceDescriptor deviceDesc {};
    deviceDesc.label = WGPUStringView("Default");
    WGPULimits requiredLimits = adapterLimits;
    
    requiredLimits.maxBufferSize                   = std::min(requiredLimits.maxBufferSize,                   (uint64_t)512  * 1024 * 1024);
    requiredLimits.maxUniformBufferBindingSize     = std::min(requiredLimits.maxUniformBufferBindingSize,     (uint64_t)16   * 1024 * 1024);
    requiredLimits.maxStorageBufferBindingSize     = std::min(requiredLimits.maxStorageBufferBindingSize,     (uint64_t)128  * 1024 * 1024);
    requiredLimits.maxTextureDimension2D           = std::min(requiredLimits.maxTextureDimension2D,           (uint32_t)4096);
    requiredLimits.maxTextureArrayLayers           = std::min(requiredLimits.maxTextureArrayLayers,           (uint32_t)256);
    requiredLimits.maxComputeWorkgroupStorageSize  = std::min(requiredLimits.maxComputeWorkgroupStorageSize,  (uint32_t)16   * 1024);
    
    deviceDesc.requiredLimits = &requiredLimits;
    deviceDesc.defaultQueue.label = WGPUStringView("Default queue");
    deviceDesc.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    deviceDesc.deviceLostCallbackInfo.callback = [](
        WGPUDevice const* InDevice,
        const WGPUDeviceLostReason InReason,
        const WGPUStringView InMessage, 
        void* InData1, 
        void* InData2) 
    {
        // TODO: Recreate everything!
        LOG("Device lost: ", InReason, " | ", ToStr(InMessage));
        CHECK_ASSERT(true, "Device lost");
    };
    deviceDesc.uncapturedErrorCallbackInfo.callback = [](
        WGPUDevice const* InDevice,
        WGPUErrorType InType, 
        WGPUStringView InMessage, 
        void* InData1, 
        void* InData2)
    {
        LOG("Device error: ", InType, " | ", ToStr(InMessage));
    };
    
    bool response = false;
    WGPURequestDeviceCallbackInfo deviceCallbackInfo = {};
    deviceCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    deviceCallbackInfo.userdata1 = &device;
    deviceCallbackInfo.userdata2 = &response;
    deviceCallbackInfo.callback = [](
        const WGPURequestDeviceStatus InStatus,
        const WGPUDevice InDevice, 
        const WGPUStringView InMessage, 
        void *InUserData1, 
        void *InUserData2)
    {
        auto device = static_cast<WGPUDevice*>(InUserData1);
        if (InStatus == WGPURequestDeviceStatus_Success)
            *device = InDevice;
        else
            LOG("Could not get WebGPU device: ", Rendering::ToStr(InMessage));
        auto response = static_cast<bool*>(InUserData2);
        *response = true;
    };
    
    wgpuAdapterRequestDevice(adapter, &deviceDesc, deviceCallbackInfo);
    while (!response)
    {
#ifdef EMSCRIPTEN
        emscripten_sleep(10);
#endif
        wgpuInstanceProcessEvents(instance);
    }
    
    CHECK_ASSERT(!device, "Failed to create device");
    
    // Inspect device
    WGPUSupportedFeatures deviceFeatures = {};
    wgpuDeviceGetFeatures(device, &deviceFeatures);
    LOG("Device features:");
    for (size_t i = 0; i < deviceFeatures.featureCount; i++)
        LOG(" - ", deviceFeatures.features[i]);
    WGPULimits deviceLimits = {};
    if (wgpuDeviceGetLimits(device, &deviceLimits) == WGPUStatus_Success) 
    {
        LOG("Device limits:");
        LOG(" - maxTextureDimension1D: ", deviceLimits.maxTextureDimension1D);
        LOG(" - maxTextureDimension2D: ", deviceLimits.maxTextureDimension2D);
        LOG(" - maxTextureDimension3D: ", deviceLimits.maxTextureDimension3D);
        LOG(" - maxTextureArrayLayers: ", deviceLimits.maxTextureArrayLayers);
        LOG(" - maxVertexAttributes: ", deviceLimits.maxVertexAttributes);
    }
}

void Rendering::Context::InitQueue()
{
    RN_PROFILE();
    
    // Setup the queue
    queue = wgpuDeviceGetQueue(device);
    CHECK_ASSERT(!queue, "Failed to create queue");
    WGPUQueueWorkDoneCallbackInfo queueDoneCallbackInfo = {};
    queueDoneCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    queueDoneCallbackInfo.callback = [](
        const WGPUQueueWorkDoneStatus status,
#ifdef EMSCRIPTEN
        const WGPUStringView InMessage,
#endif
        void* pUserData1,
        void* pUserData2) 
    {
        CHECK_ASSERT(status != WGPUQueueWorkDoneStatus_Success, "Queue failed to submit work");
    };
    wgpuQueueOnSubmittedWorkDone(queue, queueDoneCallbackInfo);
}

void Rendering::Context::Deinit()
{
    RN_PROFILE();
    
    CHECK_ASSERT(!device, "Device not set")
    CHECK_ASSERT(!adapter, "Adapter not set")
    CHECK_ASSERT(!instance, "Instance not set")
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuInstanceRelease(instance);
    device = nullptr;
    adapter = nullptr;
    instance = nullptr;
    
    glfwTerminate();
}

WGPUSurface Rendering::Context::CreateWindowSurface(const Window& InWindow) const
{
    RN_PROFILE();

    LOG("Creating window surface...")
    
#ifdef EMSCRIPTEN
    
    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvasDesc = {};
    canvasDesc.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;
    canvasDesc.chain.next = nullptr;
    canvasDesc.selector = ToStr(config.CanvasID); // Match html canvas ID
    LOG("Canvas ID: ", config.CanvasID.Get());
    WGPUSurfaceDescriptor surfaceDesc = {};
    surfaceDesc.nextInChain = &canvasDesc.chain;
    WGPUSurface surface = wgpuInstanceCreateSurface(instance, &surfaceDesc);
    CHECK_ASSERT(!surface, "Failed to create surface");
    
#else
    
    WGPUSurface surface = glfwCreateWindowWGPUSurface(instance, static_cast<GLFWwindow*>(InWindow.window));
    CHECK_ASSERT(!surface, "Failed to create surface");
    
#endif
    
    return surface;
}

void Rendering::Context::ConfigureWindowSurface(const Window& InWindow)
{
    CHECK_ASSERT(!InWindow.surface, "Surface not configured!");
    surfaceCapabilities = {};
    WGPUStatus surfaceCapabilityStatus = wgpuSurfaceGetCapabilities(InWindow.surface, adapter, &surfaceCapabilities);
    CHECK_ASSERT(surfaceCapabilityStatus != WGPUStatus_Success, "Failed to get surface capabilities");
    CHECK_ASSERT(surfaceCapabilities.formatCount <= 0, "No available surface formats");
    WGPUSurfaceConfiguration surfaceConfig = {};
    surfaceConfig.format = surfaceCapabilities.formats[0]; 
    surfaceConfig.presentMode = surfaceCapabilities.presentModes[0];
    surfaceConfig.alphaMode = surfaceCapabilities.alphaModes[0];
    surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    surfaceConfig.width = InWindow.config.Resolution.Get().x;
    surfaceConfig.height = InWindow.config.Resolution.Get().y;
    surfaceConfig.device = device;
    wgpuSurfaceConfigure(InWindow.surface, &surfaceConfig);
}

WGPUSurface Rendering::Context::CreateSurface(const WGPUSurfaceConfiguration &InConfig) const
{
    RN_PROFILE();
    WGPUSurface surface = {};
    wgpuSurfaceConfigure(surface, &InConfig);
    CHECK_ASSERT(!surface, "Failed to create surface");
    return surface;
}

WGPUShaderModule Rendering::Context::CreateShader(const WGPUShaderModuleDescriptor &InDesc) const
{
    RN_PROFILE();
    auto shader = wgpuDeviceCreateShaderModule(device, &InDesc);
    CHECK_ASSERT(!shader, "Failed to create shader module");
    return shader;
}

WGPURenderPipeline Rendering::Context::CreatePipeline(const WGPURenderPipelineDescriptor &InDesc) const
{
    RN_PROFILE();
    // TODO: wgpuDeviceCreateRenderPipelineAsync
    auto pipeline = wgpuDeviceCreateRenderPipeline(device, &InDesc);
    CHECK_ASSERT(!pipeline, "Failed to create pipeline");
    return pipeline;
}

WGPUCommandEncoder Rendering::Context::CreateEncoder(const WGPUCommandEncoderDescriptor &InDesc) const
{
    RN_PROFILE();
    auto encoder = wgpuDeviceCreateCommandEncoder(device, &InDesc);
    CHECK_ASSERT(!encoder, "Failed to create command encoder");
    return encoder;
}

uint64 GetAlignedBufferSize(const uint64 InSize)
{
    constexpr uint64 kWriteBufferAlignment = 4;
    auto alignUp = [](const uint64 v, const uint64 a) -> uint64
    {
        return (v + a - 1) / a * a;
    };
    return alignUp(InSize, kWriteBufferAlignment);
}

WGPUBuffer Rendering::Context::CreateBuffer(WGPUBufferDescriptor InDesc) const
{
    RN_PROFILE();
    InDesc.size = GetAlignedBufferSize(InDesc.size);
    InDesc.usage |= WGPUBufferUsage_CopyDst;
    auto buffer = wgpuDeviceCreateBuffer(device, &InDesc);
    CHECK_ASSERT(!buffer, "Failed to create buffer");
    return buffer;
}

void Rendering::Context::WriteBuffer(const WGPUBuffer &InBuffer, const void *InData, uint64 InSize) const
{
    RN_PROFILE();
    uint64 alignedSize = GetAlignedBufferSize(InSize);
    if (alignedSize == InSize)
    {
        wgpuQueueWriteBuffer(queue, InBuffer, 0, InData, InSize);
    }
    else
    {
        Vector padded(alignedSize, std::byte{0});
        std::memcpy(padded.data(), InData, InSize);
        wgpuQueueWriteBuffer(queue, InBuffer, 0, padded.data(), alignedSize);
    }
}

WGPUBindGroupLayout Rendering::Context::CreateBindGroupLayout(const Vector<WGPUBindGroupLayoutEntry> &InLayoutEntries) const
{
    RN_PROFILE();
    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.entryCount = InLayoutEntries.size();
    bindGroupLayoutDesc.entries = InLayoutEntries.data();
    bindGroupLayoutDesc.label = WGPUStringView("BindGroupLayout");
    WGPUBindGroupLayout layout = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDesc);
    CHECK_ASSERT(!layout, "Failed to create bind group layout");
    return layout;
}

WGPUPipelineLayout Rendering::Context::CreateLayout(const Vector<WGPUBindGroupLayout> &InLayoutGroups) const
{
    RN_PROFILE();
    // Create the pipeline layout
    WGPUPipelineLayoutDescriptor layoutDesc{};
    layoutDesc.bindGroupLayoutCount = InLayoutGroups.size();
    layoutDesc.bindGroupLayouts = InLayoutGroups.data();
    layoutDesc.label = WGPUStringView("PipelineLayout");
    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(device, &layoutDesc);
    CHECK_ASSERT(!layout, "Failed to create pipeline layout");
    return layout;
}

WGPUBindGroup Rendering::Context::CreateBindGroup(WGPUBindGroupLayout InLayout, const Vector<WGPUBindGroupEntry> &InEntries) const
{
    RN_PROFILE();
    WGPUBindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.layout = InLayout;
    // There must be as many bindings as declared in the layout!
    bindGroupDesc.entryCount = InEntries.size();
    bindGroupDesc.entries = InEntries.data();
    bindGroupDesc.label = WGPUStringView("BindGroup");
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);
    CHECK_ASSERT(!bindGroup, "Failed to create bindgroup");
    return bindGroup;
}

void Rendering::Context::Submit(const Vector<WGPUCommandBuffer> &InCommands) const
{
    RN_PROFILE();
    CHECK_ASSERT(!queue, "Queue not initialized");
    wgpuQueueSubmit(queue, InCommands.size(), InCommands.data());
}

void Rendering::Context::Poll()
{
    RN_PROFILE();
    
#ifndef EMSCRIPTEN 
#ifdef WEBGPU_BACKEND_DAWN
    device.tick();
#elifdef WEBGPU_BACKEND_WGPU
    wgpuDevicePoll(device, false, nullptr);
#endif
#endif
}

WGPUTexture Rendering::Context::CreateTexture(const WGPUTextureDescriptor& InDesc) const
{
    RN_PROFILE();
    const WGPUTexture tex = wgpuDeviceCreateTexture(device, &InDesc);
    CHECK_ASSERT(!tex, "Invalid texture");
    return tex;
}
