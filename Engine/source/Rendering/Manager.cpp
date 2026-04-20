#include "Manager.h"

#include "ImGuiContext.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"

void Rendering::Manager::Init()
{
    RN_PROFILE();
    context.BeginInit({});
    window.Open({});
    context.EndInit(window);
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
    auto& targets = viewport.GetTargets();
    
    /*
    list.Begin("Scene");
    sceneRenderer.Render(list, viewport);
    list.End();
    
    list.Begin("Resolve");
    buffers.GetGroup(0).Set(0, targets.msaaFrame);
    buffers.GetGroup(0).Set(1, targets.msaaNormals);
    Command resolveCommand("MultisampleResolve");
    resolveCommand.targets = { &targets.frame, &targets.normals };
    resolveCommand.material = resolve;
    resolveCommand.buffers = &buffers;
    list.Add(resolveCommand);
    list.End();
    */
    
    // Post processing...
    
    list.Begin("Viewport");
    
    /*
    buffers.GetGroup(0).Set(0, targets.frame);
    Command blitCommand("Blit");
    blitCommand.targets = { &windowTarget };
    blitCommand.material = blit;
    blitCommand.buffers = &buffers;
    list.Add(blitCommand);
    */
    
    list.Add(ImGuiContext::Command(windowTarget));
    list.End(); 
    
    // Present and wait
    list.Submit();
    window.Present(InRun);
    
    ImGuiContext::BeginFrame();
    
    return pacer.Pace();
}
