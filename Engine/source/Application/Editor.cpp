#include "Editor.h"

void Application::Editor::Init()
{
    log.Init();
    panels.Init();
    assetBrowser.Init();
}

void Application::Editor::Tick()
{
    
}

void Application::Editor::Deinit()
{
    panels.Deinit();
}
