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
        BASE_TYPE_INFO(Instance);
        
    public:
        ~Instance() override = default;

        virtual void Init();
        virtual void Deinit();
        virtual void Logic(double InDelta);
        virtual void Frame();
        
        virtual bool IsEditor() const { return false; }
        bool IsFreecam() const { return editorCamera.IsControlling(); }

        Rendering::Scene& GetRenderScene();
        EditorCamera& GetEditorCamera() { return editorCamera; }

    protected:
        Utility::Time time = {};
        Menu::Manager menus = {};
        Input::Manager input = {};
        
        EditorCamera editorCamera = {};
        
    private:
        Rendering::Scene renderScene = {};
        InstanceEventManager eventManager = {};
    };
}
