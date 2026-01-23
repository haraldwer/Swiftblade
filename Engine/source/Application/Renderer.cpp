#include "Renderer.h"

void Application::Renderer::Init()
{
    context.Init({});
    window.Open(context, {});
    pacer = Rendering::FramePacer();
}

void Application::Renderer::Deinit()
{
    window.Close();
    context.Deinit();
}

int Application::Renderer::Frame(bool& InRun)
{
    window.Frame(InRun);
    window.Present();
    return pacer.Pace();
}
