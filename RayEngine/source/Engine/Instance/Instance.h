#pragma once

#include "Editor/EditorCamera.h"
#include "EventScopes.h"
#include "Input/Manager.h"
#include "Menu/Manager.h"
#include "Rendering/RenderScene.h"

namespace Engine
{
    class Instance : public Utility::Singelton<Instance>
    {
    public:
        ~Instance() override = default;

        virtual void Init();
        virtual void Deinit();
        virtual void Logic(double InDelta);
        virtual void Frame();
        
        virtual bool IsEditor() const { return false; }

        Rendering::RenderScene& GetRenderScene();

    protected:
        Utility::Time Time;
        Menu::Manager Menus;
        Input::Manager Input;
        
        EditorCamera EditorCamera;
        
    private:
        Rendering::RenderScene RenderScene;
        InstanceEventManager EventManager;

    };
}
