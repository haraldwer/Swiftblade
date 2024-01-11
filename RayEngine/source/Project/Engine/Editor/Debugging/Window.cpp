#include "Window.h"
#include "Manager.h"

Debug::Window::Window()
{
    Debug::Manager::Get().Register(this); 
}

Debug::Window::~Window()
{
    Debug::Manager::Get().Unregister(this); 
}
