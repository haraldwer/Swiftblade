#pragma once
#include "../Editor/Launcher/Launcher.h"
#include "Instance/InstanceManager.h"
#include "Physics/Manager.h"
#include "Resource/Manager.h"

namespace Engine
{
    class Manager
    {
    public:
        void Init();
        void Tick();
        void Frame();
        void Deinit();

    private:
        InstanceManager instances;
        GlobalEventManager events;
        Resource::Manager resources;
        Physics::Persistent physics;
    };
}
