#pragma once
#include "Engine/ECS/Manager.h"
#include "Engine/Input/Manager.h"
#include "Engine/Menu/Manager.h"
#include "Engine/Rendering/Scene.h"
#include "Utility/Singelton.h"
#include "Utility/Time/Time.h"

namespace Engine
{
    class Instance : public Utility::Singelton<Instance>
    {
    public:
        virtual ~Instance() = default;
        
        virtual void Init()
        {
            Input.Init();
        }
        
        virtual void Deinit()
        {
            Menus.Clear();
        }
        
        virtual void Update(double InDelta)
        {
            Time.Tick(InDelta);
            Input.Update(); 
            Menus.Update(InDelta);
        }
        
        virtual void Draw() {}
        
        virtual bool IsEditor() const { return false; } 
        
        Rendering::LogicScene& GetRenderScene()
        {
            return *reinterpret_cast<Rendering::LogicScene*>(&RenderScene);
        }

    protected:
        Utility::Time Time;
        Menu::Manager Menus;
        Input::Manager Input;
        
    private:
        Rendering::Scene RenderScene;
    };
}
