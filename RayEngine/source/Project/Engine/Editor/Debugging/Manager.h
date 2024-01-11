#pragma once
#include "Window.h"
#include "Utility/Singelton.h"

namespace Debug
{
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void DrawDebugUI() const;
        
        void Register(Window* InWindow);
        void Unregister(Window* InWindow);

    private:
        Set<Window*> Windows; 
    };
}
