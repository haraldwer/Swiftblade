#pragma once

#include "Scene.h"
#include "Utility/Singelton.h"

namespace Rendering
{
    class Renderer : public Utility::Singelton<Renderer>
    {

    public:

        void Init(); 
        void Render();

        void ClearScenes(); 
        void PushScene(const LogicScene& InScene);

    private:

        std::vector<Scene> Scenes; 

    };
}

