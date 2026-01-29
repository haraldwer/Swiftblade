#pragma once
#include "ContextConfig.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <webgpu/webgpu.hpp>

namespace Rendering
{
    class CommandList;
    class Window;

    class Context : public Utility::Singelton<Context, true>
    {
        friend class ImGuiContext;
    public:
        void Init(const ContextConfig& InConfig);
        void Deinit();
        
        wgpu::Surface CreateWindowSurface(const Window& InWindow) const;
        wgpu::Surface CreateSurface(const wgpu::SurfaceConfiguration& InConfig) const;
        wgpu::ShaderModule CreateShader(const wgpu::ShaderModuleDescriptor& InDesc) const;
        wgpu::RenderPipeline CreatePipeline(const wgpu::RenderPipelineDescriptor& InDesc) const;
        wgpu::CommandEncoder CreateEncoder(const wgpu::CommandEncoderDescriptor& InDesc) const;
        wgpu::Buffer CreateBuffer(const wgpu::BufferDescriptor& InDesc);
        wgpu::Buffer UploadBuffer(const String& InLabel, const void* InData, uint64 InSize, wgpu::BufferUsage InUsage);
        void Submit(const Vector<wgpu::CommandBuffer>& InCommands) const;
        void Poll();

    private:
        ContextConfig config;
        
        void InitGLFW();
        void CreateInstance();
        void GetAdapter();
        void GetDevice();
        void GetQueue();
        
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Device device;
        wgpu::Queue queue;
        
        // TODO: Store limits! 
    };
}
