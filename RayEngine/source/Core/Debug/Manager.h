#pragma once

#include "Config.h"
#include "Window.h"
#include "Core/Utility/Singelton.h"

namespace Debug
{
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Init();
        void Deinit();
        void Logic();
        void Frame(double InDeltaTime);
        
        bool Enabled() const { return Config.DebugEnabled; }
        
        void Register(Window* InWindow);
        void Unregister(const Window* InWindow);
        
        bool IsOpen(const String& InWindow) const;
        void SetOpen(const String& InWindow, bool InOpen);

    private:
        Set<Window*> PendingRegister;
        Map<String, Vector<Window*>> Windows;
        Map<const Window*, String> WindowToName;
        Config Config;

        // Logic tick counter
        int LogicCounter = 0;
        double TPF = 0.0; 
    };
}
