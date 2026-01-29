#include "Manager.h"

#include "FramePacer.h"

void Editor::Manager::Init()
{
    log.Init();
    panels.Init();
    assetBrowser.Init();
    launcher.Init();
}

void Editor::Manager::Tick()
{
    panels.Logic();
}

void Editor::Manager::Frame()
{
    double dt = Rendering::FramePacer::Get().FrameDelta();
    panels.Frame(dt);
}

void Editor::Manager::Deinit()
{
    assetBrowser.Deinit();
    launcher.Deinit();
    panels.Deinit();
}
