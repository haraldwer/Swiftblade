#include "Window.h"

#include "Rendering2/Context/Context.h"

void Rendering::Window::Open(const Context& InContext, const WindowConfig& InConfig)
{
    CHECK_ASSERT(window, "Window already opened")
    auto res = InConfig.Resolution.Get();
    CHECK_ASSERT(res.Min() <= 0, "Invalid resolution");
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(
        res.x, 
        res.y, 
        InConfig.Title.Get().c_str(), 
        nullptr, 
        nullptr);
    assert(window && "Failed to create window");
    
    // TODO: Handle glfw events!
    // Window resizing etc
    surface = InContext.CreateSurface(res, window);
}

void Rendering::Window::Close()
{
    surface.release();
    glfwDestroyWindow(window);
    window = nullptr;
}

void Rendering::Window::Frame(bool& InRun)
{
    if (glfwWindowShouldClose(window))
        InRun = false;
    glfwPollEvents();
}

void Rendering::Window::Present()
{
#ifndef __EMSCRIPTEN__
    wgpu::Status status = surface.present(); // TODO: Might fail!
    assert(status == wgpu::Status::Success && "Failed to present frame");
#endif
}
