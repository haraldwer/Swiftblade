#pragma once
#include "ContextConfig.h"
#include "webgpu/webgpu.h"

namespace Rendering
{
    class CommandList;
    class Window;

    class Context : public Utility::Singelton<Context, true>
    {
        friend class ImGuiContext;
    public:
        void BeginInit(const ContextConfig& InConfig);
        void EndInit(const Window& InWindow);
        void Deinit();
        
        WGPUSurface CreateWindowSurface(const Window& InWindow) const;
        void ConfigureWindowSurface(const Window& InWindow);
        WGPUSurface CreateSurface(const WGPUSurfaceConfiguration& InConfig) const;
        WGPUShaderModule CreateShader(const WGPUShaderModuleDescriptor& InDesc) const;
        WGPURenderPipeline CreatePipeline(const WGPURenderPipelineDescriptor& InDesc) const;
        WGPUCommandEncoder CreateEncoder(const WGPUCommandEncoderDescriptor& InDesc) const;
        
        WGPUBuffer CreateBuffer(WGPUBufferDescriptor InDesc) const;
        void WriteBuffer(const WGPUBuffer& InBuffer, const void* InData, uint64 InSize) const;
        
        WGPUBindGroupLayout CreateBindGroupLayout(const Vector<WGPUBindGroupLayoutEntry>& InLayoutEntries) const;
        WGPUPipelineLayout CreateLayout(const Vector<WGPUBindGroupLayout>& InLayoutGroups) const;
        WGPUBindGroup CreateBindGroup(WGPUBindGroupLayout InLayout, const Vector<WGPUBindGroupEntry>& InEntries) const;
        void Submit(const Vector<WGPUCommandBuffer>& InCommands) const;
        void Poll();

        WGPUTexture CreateTexture(const WGPUTextureDescriptor& InDesc) const;


    private:
        ContextConfig config;
        
        void InitGLFW();
        void InitInstance();
        void InitAdapter(const Window& InWindow);
        void InitDevice();
        void InitQueue();
        
        WGPUInstance instance;
        WGPUAdapter adapter;
        WGPUDevice device;
        WGPUQueue queue;
        
        // TODO: Store limits!
        WGPULimits adapterLimits;
        WGPUSupportedFeatures adapterFeatures;
        WGPUAdapterInfo adapterInfo;
        WGPUSurfaceCapabilities surfaceCapabilities;
    };
}
