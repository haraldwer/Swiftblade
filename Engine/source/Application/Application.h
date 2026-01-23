#pragma once

#include "Audio.h"
#include "Editor.h"
#include "Engine.h"
#include "Renderer.h"

#define EDITOR_ENABLED

namespace Application
{
    class App
    {
    public:
        void Init();
        void Run();
        void Deinit();
        
    private:
        void Frame();
        
        bool run = true;
        Engine engine;
        Renderer renderer;
        Audio audio;
        
#ifdef EDITOR_ENABLED
        Editor editor;
#endif
    };
}
