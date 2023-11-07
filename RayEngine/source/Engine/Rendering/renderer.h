#pragma once

#include "Utility/Singelton.h"
#include "Scene.h"
#include "Settings.h"

namespace Rendering
{
    class Renderer : public Utility::Singelton<Renderer, true>
    {

    public:

        void Init(); 
        void Deinit();
        
        bool BeginRender();
        void RenderScenes(double InDelta);
        void EndRender(); 
        
        void Clear(); 
        void Push(const Scene& InScene);

        void ApplySettings(const Settings& InSettings);

    private:

        Settings CurrentSettings; 
        Vector<Scene> Scenes; 

    };
}

