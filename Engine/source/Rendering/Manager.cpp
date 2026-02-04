#include "Manager.h"

#include "ImGuiContext.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"

void Rendering::Manager::Init()
{
    RN_PROFILE();
    context.Init({});
    window.Open({});
    ImGuiContext::Init(window, context);
    pipelineCache.Init();
    viewport.Init(window.Size());
}

void Rendering::Manager::Deinit()
{
    RN_PROFILE();
    pipelineCache.Deinit();
    ImGuiContext::Deinit();
    window.Close();
    context.Deinit();
    buffers.Clear();
}

int Rendering::Manager::Frame(bool& InRun)
{
    RN_PROFILE();
    
    ImGuiContext::EndFrame();

    RenderTarget& windowTarget = window.BeginFrame();
    viewport.Resize(window.Size());
    
    list.Begin("Scene");
    sceneRenderer.Render(list, viewport);
    list.End();
    
    list.Begin("Viewport");
    
    // Maybe blit
    buffers.GetGroup(1).Set(0, viewport.GetTargets().frame);
    Command command("Blit");
    command.targets = { &windowTarget };
    command.material = blit;
    list.Add(command);
    
    list.Add(ImGuiContext::Command(windowTarget));
    list.End();
    
    // Present and wait
    list.Submit();
    window.Present(InRun);
    
    ImGuiContext::BeginFrame();
    
    return pacer.Pace();
}
