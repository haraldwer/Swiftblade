#pragma once

#include "Core/Utility/EventScopes.h"
#include "Utility/FreeCamera.h"
#include "Input/Manager.h"
#include "Menu/Manager.h"

namespace Engine
{
    class Instance : public Utility::Singelton<Instance>
    {
        BASE_CLASS_INFO(Instance);
        
    public:
        ~Instance() override = default;

        virtual void Init();
        virtual void Deinit();
        virtual void Logic(double InDelta);
        virtual void Frame();
        
        virtual bool IsEditor() const { return false; }
        bool IsFreecam() const { return freeCamera.IsControlling(); }

        FreeCamera& GetFreeCamera() { return freeCamera; }

    protected:
        Utility::Time time = {};
        Menu::Manager menus = {};
        Input::Manager input = {};
        FreeCamera freeCamera = {};
        
    private:
        InstanceEventManager eventManager = {};
    };
}
