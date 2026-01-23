#pragma once
#include "WindowConfig.h"

namespace Rendering
{
    class Context;

    class Window
    {
    public:
        void Open(const Context& InContext, const WindowConfig& InConfig);
        void Close();
        void Frame(bool& InRun);
        void Present();


    private:
        WindowConfig config = {};
        
        GLFWwindow* window = nullptr;
        wgpu::Surface surface;
    };
}
