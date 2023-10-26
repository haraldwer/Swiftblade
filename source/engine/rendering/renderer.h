#pragma once

#include "Utility/Singelton.h"
#include "Scene.h"

namespace Rendering
{
    class Renderer : public Utility::Singelton<Renderer>
    {

    public:

        void Init(); 
        void Render(double InDelta);

        void Clear(); 
        void Push(const Scene& InScene);

    private:

        Vector<Scene> Scenes; 

    };
}

