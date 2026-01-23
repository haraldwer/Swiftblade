#include "Engine.h"

void Application::Engine::Init()
{
    Utility::File::SetWorkingDir();
    
    physics.Init();
    launcher.Init();
}

void Application::Engine::Tick()
{
    
}

void Application::Engine::Frame()
{
    
}

void Application::Engine::Deinit()
{
    launcher.Deinit();
    physics.Deinit();
}
