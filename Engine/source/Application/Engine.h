#pragma once
#include "Instance/Launcher.h"
#include "Instance/Manager.h"
#include "Physics/Manager.h"
#include "Resource/Manager.h"

namespace Application
{
    class Engine
    {
    public:
        void Init();
        void Tick();
        void Frame();
        void Deinit();

    private:
        ::Engine::Manager instances;
        ::Engine::Launcher launcher;
        ::Engine::GlobalEventManager events;
        Resource::Manager resources;
        Physics::Persistent physics;
    };
}
