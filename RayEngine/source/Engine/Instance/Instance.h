#pragma once

#include "Core/Utility/EventScopes.h"
#include "Editor/EditorCamera.h"
#include "Input/Manager.h"
#include "Menu/Manager.h"
#include "Rendering/Scene/Scene.h"

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

        Rendering::Scene& GetRenderScene();
        EditorCamera& GetEditorCamera() { return EditorCamera; }

    protected:
        Utility::Time Time;
        Menu::Manager Menus;
        Input::Manager Input;
        
        EditorCamera EditorCamera;
        
    private:
        Rendering::Scene RenderScene;
        InstanceEventManager EventManager;

    };
}
