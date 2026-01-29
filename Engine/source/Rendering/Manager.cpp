#include "Manager.h"

#include "ImGuiContext.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"
#include "Resources/Material.h"

void Rendering::Manager::Init()
{
    RN_PROFILE();
    context.Init({});
    window.Open({});
    ImGuiContext::Init(window, context);
    pipelineCache.Init();
}

void Rendering::Manager::Deinit()
{
    RN_PROFILE();
    pipelineCache.Deinit();
    ImGuiContext::Deinit();
    window.Close();
    context.Deinit();
}

int Rendering::Manager::Frame(bool& InRun)
{
    RN_PROFILE();
    
    ImGuiContext::EndFrame();
    
    auto windowTarget = window.BeginFrame();
    
    list.Begin("Main");
    Command command("Main");
    command.material = rm;
    command.targets = { &windowTarget };
    command.clear = true;
    command.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    command.depthTarget = nullptr;
    command.transforms = {}; // Assumes one mesh
    command.model = m; // Assumes fullscreen pass
    command.uniforms = {}; // Contains textures and buffers
    list.Add(command);
    list.Add(ImGuiContext::Command(windowTarget));
    list.End();
    
    // Present and wait
    list.Submit();
    window.Present(InRun);
    
    ImGuiContext::BeginFrame();
    
    return pacer.Pace();
}
