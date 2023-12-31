#pragma once
#include "Engine/ECS/Manager.h"
#include "Engine/Menu/Manager.h"
#include "Engine/Rendering/Scene.h"
#include "Utility/Singelton.h"
#include "Utility/Time/Time.h"

namespace Engine
{
    class InstanceBase : public Utility::Singelton<InstanceBase>
    {
    public:
        virtual ~InstanceBase() = default;
        virtual void Init() = 0;
        virtual void Deinit() = 0;
        virtual void Update(double InDelta) {}
        virtual void DrawUI() {}
        virtual void DrawDebug() {}
        
        virtual bool IsEditor() const { return false; } 
        
        Rendering::LogicScene& GetRenderScene()
        {
            return *reinterpret_cast<Rendering::LogicScene*>(&RenderScene);
        }


    private:
        Rendering::Scene RenderScene;
    };

    class Instance : public InstanceBase
    {
        
    public:
        
        void Init() override
        {
            ECS.Init();
        }
        
        void Deinit() override
        {
            ECS.Deinit();
            Menus.Clear();
        }

    protected:
        Utility::Time Time;
        ECS::Manager ECS;
        Menu::Manager Menus; 
    };
}
