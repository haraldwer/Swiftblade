#pragma once

#include "FramePacer.h"
#include "Uniforms/UniformBuffer.h"
#include "Commands/CommandList.h"
#include "Context/Context.h"
#include "Pipeline/PipelineCache.h"
#include "Scene/SceneRenderer.h"
#include "Viewport/Viewport.h"
#include "Window/Window.h"

// If using Dawn
//#define WEBGPU_BACKEND_DAWN
// If using wgpu-native
//#define WEBGPU_BACKEND_WGPU
// If using emscripten
//#define WEBGPU_BACKEND_EMSCRIPTEN
// If built for web
//#if (EMSCRIPTEN)

namespace Rendering
{
    struct Scene;

    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Init();
        void Deinit();
        int Frame(bool& InRun);
        
        Window& GetWindow() { return window; }
        Viewport& GetViewport() { return viewport; }
        
        bool IsViewportClickable() const { return true; }

    private:
        Window window;
        Viewport viewport;
        Context context;
        FramePacer pacer;
        PipelineCache pipelineCache;
        CommandList list;
        BufferCollection buffers;
        SceneRenderer sceneRenderer;
        
        ResRM blit = ResRM("Shaders/SH_Blit.wgsl");
    };
}
