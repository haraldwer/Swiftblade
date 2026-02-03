#include "Manager.h"

#include "ImGuiContext.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"
#include "Scene/Instances/CameraInstance.h"

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
    buffers.Clear();
}

int Rendering::Manager::Frame(bool& InRun)
{
    RN_PROFILE();
    
    ImGuiContext::EndFrame();
    
    auto windowTarget = window.BeginFrame();
    viewport.Resize(window.Size());
    
    list.Begin("Main");
    
    for (auto& entry : queue.entries)
    {
        // render all the scenes :)
        // Assume default viewport
        entry.scene->Build();
        
        RenderTarget& target = windowTarget; // Depends on entry
        
        
        auto& cam = entry.scene->camera;
        Vec2F res = windowTarget.GetSize().To<float>();
        auto view = cam.GetViewMatrix();
        auto proj = cam.GetProjectionMatrix(res);
        auto viewproj = Mat4F::GetInverse(view) * proj;
        
        auto& buffer = buffers.GetGroup(0);
        buffer.Set(0, viewproj);
        
        Command command("MainCommand");
        command.material = rm;
        command.targets = { &windowTarget };
        command.clear = true;
        command.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
        command.depthTarget = nullptr;
        command.transforms = {}; // Assumes one mesh
        command.model = m; // Assumes fullscreen pass
        command.uniforms = {}; // Contains textures and buffers
        command.buffers = &buffers;
        list.Add(command);
    }
    queue.entries.clear();
    
    
    list.Add(ImGuiContext::Command(windowTarget));
    
    list.End();
    
    // Present and wait
    list.Submit();
    window.Present(InRun);
    
    ImGuiContext::BeginFrame();
    
    return pacer.Pace();
}
