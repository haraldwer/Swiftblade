#pragma once

#include "Config.h"
#include "Window.h"
#include "Utility/Singelton.h"

namespace Debug
{
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Init();
        void Deinit();
        
        void DrawDebugWindow();
        bool Enabled() const { return DebugEnabled; }
        
        void Register(Window* InWindow);
        void Unregister(Window* InWindow);

    private:
        bool DebugEnabled = true;
        Set<Window*> PendingRegister;
        Map<String, Window*> NameToWindow;
        Map<Window*, String> WindowToName;
        Config Config;
    };
}
