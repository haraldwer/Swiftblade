#pragma once

#include "FramePacer.h"
#include "Commands/CommandList.h"
#include "Context/Context.h"
#include "Pipeline/PipelineCache.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
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
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Init();
        void Deinit();
        int Frame(bool& InRun);
        bool IsViewportClickable() const { return true; }

    private:
        Window window;
        Context context;
        FramePacer pacer;
        PipelineCache pipelineCache;
        CommandList list;
        
        ResRM rm = ResRM("RM_Test.json");
        ResModel m = ResModel("Sponza/sponza.obj");
    };
}
