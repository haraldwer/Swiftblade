#pragma once
#include "Instance/InstanceManager.h"
#include "Physics/Manager.h"
#include "Resource/Manager.h"
#include "Tasks/Manager.h"

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
        Tasks::Manager tasks;
        InstanceManager instances;
        GlobalEventManager events;
        Resource::Manager resources;
        Physics::Persistent physics;
    };
}
