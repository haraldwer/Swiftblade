#include "Manager.h"

#include "FramePacer.h"

void Engine::Manager::Init()
{
    tasks.Init();
    physics.Init();
}

void Engine::Manager::Tick()
{
    double delta = Rendering::FramePacer::Get().TickDelta();
    if (Instance* instance = instances.Top())
        instance->Tick(delta);
    instances.Tick();
}

void Engine::Manager::Frame()
{
    if (Instance* instance = instances.Top())
        instance->Frame();
    resources.Frame();
    
    // TODO: Submit frame for rendering?
}

void Engine::Manager::Deinit()
{
    physics.Deinit();
    resources.Deinit();
    tasks.Deinit();
}
