#pragma once

#include "Audio/Manager.h"
#include "Editor/Manager.h"
#include "Engine/Manager.h"
#include "Rendering/Manager.h"

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
        
#ifdef EDITOR_ENABLED
        Editor::Manager editor;
#endif
        Engine::Manager engine;
        Rendering::Manager renderer;
        Audio::Manager audio;
        
        bool run = true;
    };
}
