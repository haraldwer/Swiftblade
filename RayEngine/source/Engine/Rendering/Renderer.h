#pragma once

#include "Utility/Singelton.h"
#include "Scene.h"
#include "Config.h"

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

        void ApplyConfig(const Config& InConfig);

    private:

        Config CurrConfig; 
        Vector<Scene> Scenes; 

    };
}

