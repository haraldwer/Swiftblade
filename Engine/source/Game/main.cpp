
#include "webgpu/webgpu.hpp"

// If using Dawn
//#define WEBGPU_BACKEND_DAWN
// If using wgpu-native
//#define WEBGPU_BACKEND_WGPU
// If using emscripten
//#define WEBGPU_BACKEND_EMSCRIPTEN
// If built for web
//#if (EMSCRIPTEN)


// Request adapter synchronously
WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const* options) 
{
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;
    
    // OUTDATED
    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void * pUserData1, void * pUserData2) 
    {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData1);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            std::cout << "Could not get WebGPU adapter: " << message.data << std::endl;
        }
        userData.requestEnded = true;
    };
    
    WGPURequestAdapterCallbackInfo adapterRequestCallback = {
        .nextInChain = nullptr,
        .mode = WGPUCallbackMode_AllowProcessEvents,
        .callback = onAdapterRequestEnded,
        .userdata1 = (void*)&userData,
        .userdata2 = nullptr
        
    };
    
    wgpuInstanceRequestAdapter(
        instance,
        options,
        adapterRequestCallback
    );

    // We wait until userData.requestEnded gets true
#ifdef __EMSCRIPTEN__
    while (!userData.requestEnded) {
        emscripten_sleep(100);
    }
#endif

    assert(userData.requestEnded);

    return userData.adapter;
}

int main()
{
    printf("Hello world!");
    
    
    
    // Step 1: Create WGPU instance!
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    WGPUInstance instance = wgpuCreateInstance(nullptr);
#else
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    WGPUInstance instance = wgpuCreateInstance(&desc);
#endif
    
    if (!instance)
    {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }
    std::cout << "WGPU instance: " << instance << std::endl;
    wgpuInstanceRelease(instance);
    
    
    
    // Step 2: Request Adapter and create device
    // Adapter = the gpu
    std::cout << "Requesting adapter..." << std::endl;
    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    WGPUAdapter adapter = requestAdapter(instance, &adapterOpts);
    std::cout << "Got adapter: " << adapter << std::endl;
    wgpuAdapterRelease(adapter);
    
    
    
    // Step 3: Get adapter limits
#ifndef __EMSCRIPTEN__
    WGPULimits supportedLimits = {};
    supportedLimits.nextInChain = nullptr;
#ifdef WEBGPU_BACKEND_DAWN
    WGPUStatus status = wgpuAdapterGetLimits(adapter, &supportedLimits);
#else
    WGPUStatus status = wgpuAdapterGetLimits(adapter, &supportedLimits);
#endif
    if (status == WGPUStatus_Success) {
        std::cout << "Adapter limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
    }
#endif
    
    
    
    // Step 4: Get features 
    // OUTDATED
    WGPUSupportedFeatures features;
    wgpuAdapterGetFeatures(adapter, &features);
    std::cout << "Adapter features:" << std::endl;
    std::cout << std::hex; // Write integers as hexadecimal to ease comparison with webgpu.h literals
    for (size_t i = 0; i < features.featureCount; i++) {
        std::cout << " - 0x" << features.features[i] << std::endl;
    }
    std::cout << std::dec; // Restore decimal numbers
    
    
    
    // Step 5: Get properties
    //WGPUAdapterProperties properties = {};
    //properties.nextInChain = nullptr;
    //wgpuAdapterGetProperties(adapter, &properties);
    //std::cout << "Adapter properties:" << std::endl;
    //std::cout << " - vendorID: " << properties.vendorID << std::endl;
    //if (properties.vendorName) {
    //    std::cout << " - vendorName: " << properties.vendorName << std::endl;
    //}
    //if (properties.architecture) {
    //    std::cout << " - architecture: " << properties.architecture << std::endl;
    //}
    //std::cout << " - deviceID: " << properties.deviceID << std::endl;
    //if (properties.name) {
    //    std::cout << " - name: " << properties.name << std::endl;
    //}
    //if (properties.driverDescription) {
    //    std::cout << " - driverDescription: " << properties.driverDescription << std::endl;
    //}
    //std::cout << std::hex;
    //std::cout << " - adapterType: 0x" << properties.adapterType << std::endl;
    //std::cout << " - backendType: 0x" << properties.backendType << std::endl;
    //std::cout << std::dec; // Restore decimal numbers
    
    
    
    
    
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