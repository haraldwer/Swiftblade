#pragma once

#include "Rendering2/FramePacer.h"
#include "Rendering2/Context/Context.h"
#include "Rendering2/Window/Window.h"

// If using Dawn
//#define WEBGPU_BACKEND_DAWN
// If using wgpu-native
//#define WEBGPU_BACKEND_WGPU
// If using emscripten
//#define WEBGPU_BACKEND_EMSCRIPTEN
// If built for web
//#if (EMSCRIPTEN)

namespace Application
{
    class Renderer
    {
    public:
        void Init();
        void Deinit();
        int Frame(bool& InRun);

    private:
        Rendering::Window window;
        Rendering::Context context;
        Rendering::FramePacer pacer;
        
        wgpu::Queue queue;
        wgpu::ShaderModule shaderModule;
        wgpu::RenderPipeline pipeline;
    };
}
