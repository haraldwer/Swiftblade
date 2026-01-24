#pragma once

#include "Core/Utility/EventScopes.h"
#include "FreeCamera.h"
#include "Input/Manager.h"
#include "Menu/Manager.h"
#include "Rendering/Scene/Scene.h"

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

        Rendering::Scene& GetRenderScene();
        FreeCamera& GetFreeCamera() { return freeCamera; }

    protected:
        Utility::Time time = {};
        Menu::Manager menus = {};
        Input::Manager input = {};
        FreeCamera freeCamera = {};
        
    private:
        Rendering::Scene renderScene = {};
        InstanceEventManager eventManager = {};
    };
}
