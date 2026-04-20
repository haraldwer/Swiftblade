#include "Window.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Rendering/Context/Context.h"

void Rendering::Window::Open(const WindowConfig& InConfig)
{
    RN_PROFILE();
    CHECK_ASSERT(window, "Window already opened")
    auto res = InConfig.Resolution.Get();
    CHECK_ASSERT(res.Min() <= 0, "Invalid resolution");
    config = InConfig;
    
    LOG("Creating glfw window")
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(
        res.x, 
        res.y, 
        InConfig.Title.Get().c_str(), 
        nullptr,
        nullptr);
    CHECK_ASSERT(!window, "Failed to create window");
    input.Init(window);
    
    surface = Context::Get().CreateWindowSurface(*this);
    
    // TODO: Handle glfw events!
    // Window resizing etc
}

void Rendering::Window::Close()
{
    RN_PROFILE();
    target.Deinit();
    wgpuSurfaceRelease(surface);
    surface = {};
    input.Deinit();
    glfwDestroyWindow(static_cast<GLFWwindow*>(window));
    window = nullptr;
}

Rendering::RenderTarget& Rendering::Window::BeginFrame()
{
    RN_PROFILE();
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
    CHECK_ASSERT(surfaceTexture.status > WGPUSurfaceGetCurrentTextureStatus_Timeout, "Failed to get surface texture");
    
    target.Deinit();
    target.Init(surfaceTexture.texture);
    return target;
}

void Rendering::Window::Present(bool& InRun)
{
    RN_PROFILE();
#ifndef EMSCRIPTEN
    const WGPUStatus status = wgpuSurfacePresent(surface);
    if (status != WGPUStatus_Success)
        LOG("Failed to present frame");
#endif
    
    // Also poll events
    input.Frame();
    Context::Get().Poll();
    
#ifndef EMSCRIPTEN
    glfwPollEvents();
    if (glfwWindowShouldClose(static_cast<GLFWwindow*>(window)))
        InRun = false;
#endif
    
    // Release surface texture
    target.Deinit();
    surfaceTexture.texture = {};
}

Vec2I Rendering::Window::Size() const
{
    return config.Resolution.Get();
}
