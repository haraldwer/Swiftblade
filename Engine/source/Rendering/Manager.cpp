#include "Manager.h"

#include "ImGuiContext.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"
#include "Scene/Instances/CameraInstance.h"
#include "Utility/FreeCamera.h"

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
    
    CameraInstance cam;
    cam.near = 0.1f;
    cam.far = 50.0f;
    cam.position = Vec3F::Up() * 2;
    cam.rotation = QuatF::Identity();
    Vec2F res = windowTarget.GetSize().To<float>();
    auto view = cam.GetViewMatrix();
    auto proj = cam.GetProjectionMatrix(res);
    auto viewproj = view * proj;
    
    auto& buffer = buffers.GetGroup(0);
    buffer.Set(0, viewproj);
    
    list.Begin("Main");
    
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
    
    //list.Add(ImGuiContext::Command(windowTarget));
    
    list.End();
    
    // Present and wait
    list.Submit();
    window.Present(InRun);
    
    ImGuiContext::BeginFrame();
    
    return pacer.Pace();
}
