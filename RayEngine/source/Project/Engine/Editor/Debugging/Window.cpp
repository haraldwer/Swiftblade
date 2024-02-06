#include "Window.h"
#include "Manager.h"

Debug::Window::Window()
{
    Manager::Get().Register(this); 
}

Debug::Window::~Window()
{
    Manager::Get().Unregister(this); 
}

bool Debug::Window::IsDebugWindowOpen() const
{
    return Manager::Get().IsOpen(DebugWindowName()); 
}
