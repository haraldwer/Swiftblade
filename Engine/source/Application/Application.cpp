#include "Application.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

void Application::App::Init()
{
    run = true;
    
#ifdef EDITOR_ENABLED
    editor.Init();
#endif
    
    audio.Init();
    renderer.Init();
    engine.Init();
}

void Application::App::Run()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Frame, 0, 1); 
#else
    while (run)
        Frame();
#endif
}

void Application::App::Frame()
{
    engine.Frame();
    int ticks = renderer.Frame(run);
    for (int i = 0; i < ticks; i++)
    {
        engine.Tick();
        audio.Tick();
    }
}

void Application::App::Deinit()
{
#ifdef EDITOR_ENABLED
    editor.Deinit();
#endif
    engine.Deinit();
    audio.Deinit();
    renderer.Deinit();
}
