#pragma once
#include "ContextConfig.h"

namespace Rendering
{
    class Context
    {
    public:
        void Init(const ContextConfig& InConfig);
        void Deinit();
        
        wgpu::Surface CreateSurface(const Vec2I& InResolution, GLFWwindow* InWindow = nullptr) const;

    private:
        ContextConfig config;
        
        void InitGLFW();
        void CreateInstance();
        void GetAdapter();
        void GetDevice();
        
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Device device;
        
        // TODO: Store limits!
        
    };
}
