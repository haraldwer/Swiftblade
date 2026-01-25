#include "Manager.h"

#include "Commands/Command.h"
#include "Commands/CommandList.h"
#include "Resources/Material.h"

void Rendering::Manager::Init()
{
    RN_PROFILE();
    context.Init({});
    window.Open({});
    pipelineCache.Init();
    pacer = FramePacer();
}

void Rendering::Manager::Deinit()
{
    RN_PROFILE();
    pipelineCache.Deinit();
    window.Close();
    context.Deinit();
}

int Rendering::Manager::Frame(bool& InRun)
{
    RN_PROFILE();
    auto windowTarget = window.BeginFrame();
    
    list.Begin("Main");
    Command command("Main");
    command.material = ResRM("RM_Test.json");
    command.targets = { &windowTarget };
    command.depthTarget = nullptr;
    command.transforms = {}; // Assumes one mesh
    command.model = {}; // Assumes fullscreen pass
    command.uniforms = {}; // Contains textures and buffers
    list.Add(command);
    list.End();
    
    // Present and wait
    list.Submit();
    window.Present(InRun);
    
    return pacer.Pace();
}
