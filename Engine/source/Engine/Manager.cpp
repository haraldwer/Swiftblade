#include "Manager.h"

void Engine::Manager::Init()
{
    Utility::File::SetWorkingDir();
    
    physics.Init();
}

void Engine::Manager::Tick()
{
    
}

void Engine::Manager::Frame()
{
    
}

void Engine::Manager::Deinit()
{
    physics.Deinit();
}
