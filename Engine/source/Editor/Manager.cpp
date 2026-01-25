#include "Manager.h"

void Editor::Manager::Init()
{
    log.Init();
    panels.Init();
    assetBrowser.Init();
    launcher.Init();
}

void Editor::Manager::Tick()
{
    
}

void Editor::Manager::Deinit()
{
    assetBrowser.Deinit();
    launcher.Deinit();
    panels.Deinit();
}
