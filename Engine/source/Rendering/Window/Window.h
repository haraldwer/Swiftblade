#pragma once

#include <webgpu/webgpu.hpp>

#include "WindowConfig.h"
#include "Targets/RenderTarget.h"
#include "Input.h"

namespace Rendering
{
    class Window
    {
        friend class Context;
        friend class ImGuiContext;
    public:
        void Open(const WindowConfig& InConfig);
        void Close();
        RenderTarget& BeginFrame();
        void Present(bool& InRun);

        Vec2I Size() const;

    private:
        WindowConfig config = {};
        WindowHandle window = nullptr;
        Input input = {};
        
        wgpu::Surface surface;
        wgpu::SurfaceTexture surfaceTexture;
        RenderTarget target;
    };
}
