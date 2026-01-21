

#include "glfw3webgpu.h"  

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define WEBGPU_CPP_IMPLEMENTATION
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

wgpu::Device RequestDevice(const wgpu::Instance& InInstance, const wgpu::Adapter& InAdapter, const wgpu::DeviceDescriptor& InDeviceDescriptor)
{
    // Step 6: Get device
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

int main()
{
    // Step 0: Create GLFW context and window
    int initStatus = glfwInit();
    assert(initStatus == GLFW_TRUE && "Failed to initialize glfw");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1080, 1080, "WebGPU + GLFW", nullptr, nullptr);
    assert(window && "Failed to create window");
        
        
    
    // Step 1: Create WGPU instance
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
    
    
    
    // Step 2: Request Adapter and create device
    printf("Requesting adapter...\n");
    wgpu::RequestAdapterOptions adapterOptions;
    wgpu::Adapter adapter = instance.requestAdapter(adapterOptions);
    assert(adapter && "Failed to create adapter");
    printf("Got adapter: %u\n", adapter);
    
    
    
    // Step 3: Get adapter limits
    wgpu::Limits adapterLimits;
    auto status = adapter.getLimits(&adapterLimits);
    if (status == WGPUStatus_Success) 
    {
        printf("Adapter limits:\n");
        printf(" - maxTextureDimension1D: %i\n", adapterLimits.maxTextureDimension1D);
        printf(" - maxTextureDimension2D: %i\n", adapterLimits.maxTextureDimension2D);
        printf(" - maxTextureDimension3D: %i\n", adapterLimits.maxTextureDimension3D);
        printf(" - maxTextureArrayLayers: %i\n", adapterLimits.maxTextureArrayLayers);
    }
    
    
    
    // Step 4: Get features 
    wgpu::SupportedFeatures adapterFeatures;
    adapter.getFeatures(&adapterFeatures);
    printf("Adapter features:\n");
    for (size_t i = 0; i < adapterFeatures.featureCount; i++) 
        printf(" - %i\n", adapterFeatures.features[i]);
    
    
    
    // Step 5: Get info
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
    
    
    
    // Step 6: Get device
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
    
    
    
    // Step 7: Inspect device
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
    
    
    // Step 8: Create surface
    wgpu::Surface surface = glfwCreateWindowWGPUSurface(instance, window);
    assert(surface && "Failed to create surface");
    
    
    
    // Enter the main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        
        
        // Step 9: Setup the queue
        wgpu::Queue queue = device.getQueue();
        assert(queue && "Failed to create queue");
        wgpu::QueueWorkDoneCallbackInfo queueDoneCallbackInfo;
        queueDoneCallbackInfo.callback = [](
            const WGPUQueueWorkDoneStatus status, 
            void* pUserData1,
            void* pUserData2) 
        {
            printf("Queued work finished with status: %i\n", status);
        };
        queue.onSubmittedWorkDone(queueDoneCallbackInfo);
    
        
        
        // Step 10: Build the queue 
        std::vector<wgpu::CommandBuffer> commands;
        // 9.1: Create encoder
        wgpu::CommandEncoderDescriptor encoderDesc;
        encoderDesc.label = wgpu::StringView("Default command encoder");
        wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);
        assert(encoder && "Failed to create encoder");
        // 9.2: Add instructions
        encoder.insertDebugMarker(wgpu::StringView("Do one thing"));
        encoder.insertDebugMarker(wgpu::StringView("Do another thing"));
        // 9.3: Build command
        wgpu::CommandBufferDescriptor commandBufferDesc;
        commandBufferDesc.label = wgpu::StringView("Default command buffer");
        commands.emplace_back() = encoder.finish();
        encoder.release();
    
        
        
        // Step 11: Submit the queue
        queue.submit(commands.size(), commands.data());
        for (auto& cmd : commands)
            cmd.release();
        commands.clear();
        queue.release();
        
        
        
        // Step 12: Wait a bit
        device.poll(false, nullptr);
    }
    
    
    
    // Cleanup!
    surface.release();
    device.release();
    adapter.release();
    instance.release();
    glfwDestroyWindow(window);
    glfwTerminate();
    
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