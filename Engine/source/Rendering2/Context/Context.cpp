#include "Context.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void Rendering::Context::Init(const ContextConfig& InConfig)
{
    InitGLFW();
    
    CreateInstance();
    GetAdapter();
    GetDevice();
}

void Rendering::Context::InitGLFW() 
{
    // Create GLFW context and window
    int initStatus = glfwInit();
    assert(initStatus == GLFW_TRUE && "Failed to initialize glfw");
}

void Rendering::Context::CreateInstance() 
{
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
    assert(instance && "Failed to create WGPU instance");
    printf("WGPU instance: %u\n", instance);
}

void Rendering::Context::GetAdapter() 
{
    // Request Adapter and create device
    printf("Requesting adapter...\n");
    wgpu::RequestAdapterOptions adapterOptions;
    adapter = instance.requestAdapter(adapterOptions);
    assert(adapter && "Failed to create adapter");
    printf("Adapter: %u\n", adapter);
    
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
    printf(" - vendorName: %s\n", ToStr(info.vendor));
    printf(" - architecture: %s\n", ToStr(info.architecture));
    printf(" - deviceID: %i\n", info.deviceID);
    printf(" - device: %s\n", ToStr(info.device));
    printf(" - driverDescription: %s\n", ToStr(info.description));
    printf(" - adapterType: %i\n", info.adapterType);
    printf(" - backendType: %i\n", info.backendType);
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
            printf("Could not get WebGPU device: %s", ToStr(message));
        userData.response = true;
    };
    InAdapter.requestDevice(InDeviceDescriptor, deviceCallbackInfo);
    while (!data.response)
        InInstance.processEvents();
    return data.device;
}

void Rendering::Context::GetDevice() 
{
    // Get device
    printf("Requesting device...\n");
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
                printf("Device lost: %i | %s\n", InReason, ToStr(InMessage));
                assert(false && "Device lost");
            };
    device = RequestDevice(instance, adapter, deviceDesc);
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
}

void Rendering::Context::Deinit()
{
    device.release();
    device = {};
    adapter.release();
    adapter = {};
    instance.release();
    instance = {};
    
    glfwTerminate();
}

wgpu::Surface Rendering::Context::CreateSurface(const Vec2I &InResolution, GLFWwindow *InWindow) const
{
    wgpu::Surface surface = glfwCreateWindowWGPUSurface(instance, InWindow);
    assert(surface && "Failed to create surface");
    wgpu::SurfaceCapabilities surfaceCapabilities;
    wgpu::Status surfaceCapabilityStatus = surface.getCapabilities(adapter, &surfaceCapabilities);
    assert(surfaceCapabilityStatus == wgpu::Status::Success && "Failed to get surface capabilities");
    assert(surfaceCapabilities.formatCount > 0 && "No available surface format");
    wgpu::SurfaceConfiguration surfaceConfig;
    surfaceConfig.format = surfaceCapabilities.formats[0]; 
    surfaceConfig.usage = wgpu::TextureUsage::RenderAttachment;
    surfaceConfig.width = InResolution.x;
    surfaceConfig.height = InResolution.y;
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.viewFormats = nullptr;
    surfaceConfig.device = device;
    surfaceConfig.presentMode = wgpu::PresentMode::Fifo;
    surfaceConfig.alphaMode = wgpu::CompositeAlphaMode::Opaque;
    surface.configure(surfaceConfig);
    return surface;
}
