#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <webgpu/webgpu.hpp>

#include "WindowConfig.h"
#include "Targets/RenderTarget.h"

namespace Rendering
{
    class Window
    {
        friend class Context;
    public:
        void Open(const WindowConfig& InConfig);
        void Close();
        RenderTarget& BeginFrame();
        void Present(bool& InRun);
        
    private:
        WindowConfig config = {};
        GLFWwindow* window = nullptr;
        
        wgpu::Surface surface;
        wgpu::SurfaceTexture surfaceTexture;
        RenderTarget target;
    };
}
