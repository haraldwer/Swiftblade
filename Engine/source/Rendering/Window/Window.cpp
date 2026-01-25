#include "Window.h"

#include "Rendering/Context/Context.h"

void Rendering::Window::Open(const WindowConfig& InConfig)
{
    RN_PROFILE();
    CHECK_ASSERT(window, "Window already opened")
    auto res = InConfig.Resolution.Get();
    CHECK_ASSERT(res.Min() <= 0, "Invalid resolution");
    config = InConfig;
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(
        res.x, 
        res.y, 
        InConfig.Title.Get().c_str(), 
        nullptr, 
        nullptr);
    CHECK_ASSERT(!window, "Failed to create window");
    surface = Context::Get().CreateWindowSurface(*this);
    
    // Create target!
    
    // TODO: Handle glfw events!
    // Window resizing etc
}

void Rendering::Window::Close()
{
    RN_PROFILE();
    surface.release();
    surface = {};
    glfwDestroyWindow(window);
    window = nullptr;
}

Rendering::RenderTarget& Rendering::Window::BeginFrame()
{
    RN_PROFILE();
    surface.getCurrentTexture(&surfaceTexture);
    CHECK_ASSERT(surfaceTexture.status > wgpu::SurfaceGetCurrentTextureStatus::Timeout, "Failed to get surface texture");
    
    // Create RenderTarget!
    target.texture = surfaceTexture.texture;
    return target;
}

void Rendering::Window::Present(bool& InRun)
{
    RN_PROFILE();
#ifndef __EMSCRIPTEN__
    wgpu::Status status = surface.present(); // TODO: Might fail!
    CHECK_ASSERT(status != wgpu::Status::Success, "Failed to present frame");
#endif
    
    // Also poll events
    Context::Get().Poll();
    glfwPollEvents();
    if (glfwWindowShouldClose(window))
        InRun = false;
    
    // Release surface texture
#ifdef WEBGPU_BACKEND_WGPU
    if (surfaceTexture.texture)
    {
        wgpuTextureRelease(surfaceTexture.texture);
        surfaceTexture.texture = nullptr;
    }
#endif
    target = {};
}
