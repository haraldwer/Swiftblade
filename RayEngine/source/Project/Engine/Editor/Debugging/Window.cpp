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
