#include "Application.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

void Application::App::Init()
{
    Utility::File::SetWorkingDir();
    run = true;
    
#ifdef EDITOR_ENABLED
    editor.Init();
#endif
    
    audio.Init();
    renderer.Init();
    engine.Init();
}

#ifdef EMSCRIPTEN
static Application::App* emsc_app;
void FrameFunc()
{
    CHECK_ASSERT(!emsc_app, "Invalid app!");
    emsc_app->Frame();
}
#endif

void Application::App::Run()
{
#ifdef EMSCRIPTEN
    emsc_app = this;
    emscripten_set_main_loop(FrameFunc, 0, 1);
    emsc_app = nullptr;
#else
    while (run)
        Frame();
#endif
}

void Application::App::Frame()
{
    engine.Frame();
#ifdef EDITOR_ENABLED
    editor.Frame();
#endif
    int ticks = renderer.Frame(run);
    for (int i = 0; i < ticks; i++)
    {
        engine.Tick();
        audio.Tick();
#ifdef EDITOR_ENABLED
        editor.Tick();
#endif
    }
}

void Application::App::Deinit()
{
#ifdef EDITOR_ENABLED
    editor.Deinit();
#endif
    
    audio.Deinit();
    renderer.Deinit();
    engine.Deinit();
}
