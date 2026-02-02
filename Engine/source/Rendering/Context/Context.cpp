#include "Context.h"

#include "glfw3webgpu.h"
#include "Commands/CommandList.h"

#include "Window/Window.h"

void Rendering::Context::Init(const ContextConfig& InConfig)
{
    RN_PROFILE();
    InitGLFW();
    
    CreateInstance();
    GetAdapter();
    GetDevice();
    GetQueue(); 
}

void Rendering::Context::InitGLFW() 
{
    RN_PROFILE();
    // Create GLFW context and window
    int initStatus = glfwInit();
    CHECK_ASSERT(initStatus != GLFW_TRUE, "Failed to initialize glfw");
}

void Rendering::Context::CreateInstance() 
{
    RN_PROFILE();
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
    instance = wgpu::createInstance(instanceDesc);
    CHECK_ASSERT(!instance, "Failed to create WGPU instance");
    LOG("WGPU instance: ", instance);
}

void Rendering::Context::GetAdapter() 
{
    RN_PROFILE();
    // Request Adapter and create device
    LOG("Requesting adapter...");
    wgpu::RequestAdapterOptions adapterOptions;
    adapter = instance.requestAdapter(adapterOptions);
    CHECK_ASSERT(!adapter, "Failed to create adapter");
    LOG("Adapter: ", adapter);
    
    // Get adapter limits
    wgpu::Limits adapterLimits;
    auto adapterLimitStatus = adapter.getLimits(&adapterLimits);
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
    wgpu::SupportedFeatures adapterFeatures;
    adapter.getFeatures(&adapterFeatures);
    LOG("Adapter features:");
    for (size_t i = 0; i < adapterFeatures.featureCount; i++) 
        LOG(" - ", adapterFeatures.features[i]);
    
    // Get info
    wgpu::AdapterInfo info;
    adapter.getInfo(&info);
    LOG("Adapter properties:");
    LOG(" - vendorID: ", info.vendorID);
    LOG(" - vendorName: ", ToStr(info.vendor));
    LOG(" - architecture: ", ToStr(info.architecture));
    LOG(" - deviceID: ", info.deviceID);
    LOG(" - device: ", ToStr(info.device));
    LOG(" - driverDescription: ", ToStr(info.description));
    LOG(" - adapterType: ", info.adapterType);
    LOG(" - backendType: ", info.backendType);
}

// Get device
wgpu::Device RequestDevice(const wgpu::Instance& InInstance, const wgpu::Adapter& InAdapter, const wgpu::DeviceDescriptor& InDeviceDescriptor)
{
    RN_PROFILE();
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
        RN_PROFILE();
        RequestData& userData = *static_cast<RequestData*>(pUserData1);
        if (status == WGPURequestDeviceStatus_Success)
            userData.device = device;
        else
            LOG("Could not get WebGPU device: ", Rendering::ToStr(message));
        userData.response = true;
    };
    InAdapter.requestDevice(InDeviceDescriptor, deviceCallbackInfo);
    while (!data.response)
        InInstance.processEvents();
    return data.device;
}

void Rendering::Context::GetDevice() 
{
    RN_PROFILE();
    // Get device
    LOG("Requesting device...");
    wgpu::DeviceDescriptor deviceDesc;
    deviceDesc.label = wgpu::StringView("Default");
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = wgpu::StringView("Default queue");
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
    device = RequestDevice(instance, adapter, deviceDesc);
    CHECK_ASSERT(!device, "Failed to create device");
    LOG("Got device: ", device);
    
    // Inspect device
    wgpu::SupportedFeatures deviceFeatures;
    device.getFeatures(&deviceFeatures);
    LOG("Device features:");
    for (size_t i = 0; i < deviceFeatures.featureCount; i++)
        LOG(" - ", deviceFeatures.features[i]);
    wgpu::Limits deviceLimits;
    if (device.getLimits(&deviceLimits) == wgpu::Status::Success) 
    {
        LOG("Device limits:");
        LOG(" - maxTextureDimension1D: ", deviceLimits.maxTextureDimension1D);
        LOG(" - maxTextureDimension2D: ", deviceLimits.maxTextureDimension2D);
        LOG(" - maxTextureDimension3D: ", deviceLimits.maxTextureDimension3D);
        LOG(" - maxTextureArrayLayers: ", deviceLimits.maxTextureArrayLayers);
        LOG(" - maxVertexAttributes: ", deviceLimits.maxVertexAttributes);
    }
}

void Rendering::Context::GetQueue()
{
    RN_PROFILE();
    // Setup the queue
    queue = device.getQueue();
    CHECK_ASSERT(!queue, "Failed to create queue");
    wgpu::QueueWorkDoneCallbackInfo queueDoneCallbackInfo;
    queueDoneCallbackInfo.callback = [](
        const WGPUQueueWorkDoneStatus status, 
        void* pUserData1,
        void* pUserData2) 
    {
        CHECK_ASSERT(status != WGPUQueueWorkDoneStatus_Success, "Queue failed to submit work");
    };
    queue.onSubmittedWorkDone(queueDoneCallbackInfo);
}

void Rendering::Context::Deinit()
{
    RN_PROFILE();
    
    CHECK_ASSERT(!device, "Device not set")
    CHECK_ASSERT(!adapter, "Adapter not set")
    CHECK_ASSERT(!instance, "Instance not set")
    device.release();
    device = {};
    adapter.release();
    adapter = {};
    instance.release();
    instance = {};
    
    glfwTerminate();
}

wgpu::Surface Rendering::Context::CreateWindowSurface(const Window& InWindow) const
{
    RN_PROFILE();
    wgpu::Surface surface = glfwCreateWindowWGPUSurface(instance, InWindow.window);
    CHECK_ASSERT(!surface, "Failed to create surface");
    wgpu::SurfaceCapabilities surfaceCapabilities;
    wgpu::Status surfaceCapabilityStatus = surface.getCapabilities(adapter, &surfaceCapabilities);
    CHECK_ASSERT(surfaceCapabilityStatus != wgpu::Status::Success, "Failed to get surface capabilities");
    CHECK_ASSERT(surfaceCapabilities.formatCount <= 0, "No available surface formats");
    wgpu::SurfaceConfiguration surfaceConfig;
    surfaceConfig.format = surfaceCapabilities.formats[0]; 
    surfaceConfig.usage = wgpu::TextureUsage::RenderAttachment;
    surfaceConfig.width = InWindow.config.Resolution.Get().x;
    surfaceConfig.height = InWindow.config.Resolution.Get().y;
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.viewFormats = nullptr;
    surfaceConfig.device = device;
    surfaceConfig.presentMode = wgpu::PresentMode::Immediate;
    surfaceConfig.alphaMode = wgpu::CompositeAlphaMode::Opaque;
    surface.configure(surfaceConfig);
    return surface;
}

wgpu::Surface Rendering::Context::CreateSurface(const wgpu::SurfaceConfiguration &InConfig) const
{
    RN_PROFILE();
    wgpu::Surface surface;
    surface.configure(InConfig);
    CHECK_ASSERT(!surface, "Failed to create surface");
    return surface;
}

wgpu::ShaderModule Rendering::Context::CreateShader(const wgpu::ShaderModuleDescriptor &InDesc) const
{
    RN_PROFILE();
    auto shader = device.createShaderModule(InDesc);
    CHECK_ASSERT(!shader, "Failed to create shader module");
    return shader;
}

wgpu::RenderPipeline Rendering::Context::CreatePipeline(const wgpu::RenderPipelineDescriptor &InDesc) const
{
    RN_PROFILE();
    auto pipeline = device.createRenderPipeline(InDesc);
    CHECK_ASSERT(!pipeline, "Failed to create pipeline");
    return pipeline;
}

wgpu::CommandEncoder Rendering::Context::CreateEncoder(const wgpu::CommandEncoderDescriptor &InDesc) const
{
    RN_PROFILE();
    auto encoder = device.createCommandEncoder(InDesc);
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

wgpu::Buffer Rendering::Context::CreateBuffer(wgpu::BufferDescriptor InDesc) const
{
    RN_PROFILE();
    InDesc.size = GetAlignedBufferSize(InDesc.size);
    InDesc.usage |= wgpu::BufferUsage::CopyDst;
    auto buffer = device.createBuffer(InDesc);
    CHECK_ASSERT(!buffer, "Failed to create buffer");
    return buffer;
}

void Rendering::Context::WriteBuffer(const wgpu::Buffer &InBuffer, const void *InData, uint64 InSize) const
{
    uint64 alignedSize = GetAlignedBufferSize(InSize);
    if (alignedSize == InSize)
    {
        queue.writeBuffer(InBuffer, 0, InData, InSize);
    }
    else
    {
        Vector padded(alignedSize, std::byte{0});
        std::memcpy(padded.data(), InData, InSize);
        queue.writeBuffer(InBuffer, 0, padded.data(), alignedSize);
    }
}

wgpu::BindGroupLayout Rendering::Context::CreateBindGroupLayout(const Vector<wgpu::BindGroupLayoutEntry> &InLayoutEntries) const
{
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.entryCount = InLayoutEntries.size();
    bindGroupLayoutDesc.entries = InLayoutEntries.data();
    bindGroupLayoutDesc.label = wgpu::StringView("BindGroupLayout");
    wgpu::BindGroupLayout layout = device.createBindGroupLayout(bindGroupLayoutDesc);
    CHECK_ASSERT(!layout, "Failed to create bind group layout");
    return layout;
}

wgpu::PipelineLayout Rendering::Context::CreateLayout(const Vector<wgpu::BindGroupLayout> &InLayoutGroups)
{
    // Create the pipeline layout
    wgpu::PipelineLayoutDescriptor layoutDesc{};
    layoutDesc.bindGroupLayoutCount = InLayoutGroups.size();
    layoutDesc.bindGroupLayouts = reinterpret_cast<WGPUBindGroupLayout const *>(InLayoutGroups.data());
    layoutDesc.label = wgpu::StringView("PipelineLayout");
    wgpu::PipelineLayout layout = device.createPipelineLayout(layoutDesc);
    CHECK_ASSERT(!layout, "Failed to create pipeline layout");
    return layout;
}

wgpu::BindGroup Rendering::Context::CreateBindGroup(wgpu::BindGroupLayout InLayout, const Vector<wgpu::BindGroupEntry> &InEntries)
{
    wgpu::BindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.layout = InLayout;
    // There must be as many bindings as declared in the layout!
    bindGroupDesc.entryCount = InEntries.size();
    bindGroupDesc.entries = InEntries.data();
    bindGroupDesc.label = wgpu::StringView("BindGroup");
    wgpu::BindGroup bindGroup = device.createBindGroup(bindGroupDesc);
    CHECK_ASSERT(!bindGroup, "Failed to create bindgroup");
    return bindGroup;
}

void Rendering::Context::Submit(const Vector<wgpu::CommandBuffer> &InCommands) const
{
    RN_PROFILE();
    CHECK_ASSERT(!queue, "Queue not initialized");
    queue.submit(InCommands.size(), InCommands.data());
}

void Rendering::Context::Poll()
{
    RN_PROFILE();
#if defined(WEBGPU_BACKEND_DAWN)
    device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
    if (!device.poll(false, nullptr))
        LOG("Device poll failed, might be a bad state");
#endif
}
