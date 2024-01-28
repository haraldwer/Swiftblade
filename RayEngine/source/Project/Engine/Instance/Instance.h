#pragma once
#include "Engine/EventScopes.h"
#include "Engine/Input/Manager.h"
#include "Engine/Menu/Manager.h"
#include "..\Rendering\RenderScene.h"
#include "Utility/Singelton.h"
#include "Utility/Time/Time.h"

namespace Engine
{
    class Instance : public Utility::Singelton<Instance>
    {
    public:
        ~Instance() override = default;

        virtual void Init();
        virtual void Deinit();
        virtual void Logic(double InDelta);
        virtual void Frame(double InDelta);
        
        virtual bool IsEditor() const { return false; }

        Rendering::RenderScene& GetRenderScene();

    protected:
        Utility::Time Time;
        Menu::Manager Menus;
        Input::Manager Input;
        
    private:
        Rendering::RenderScene RenderScene;
        InstanceEventManager EventManager;
    };
}
