#pragma once

#include "Rendering2/Window/Window.h"
#include "Rendering2/FramePacer.h"
#include "Rendering2/Context/Context.h"

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
    };
}
