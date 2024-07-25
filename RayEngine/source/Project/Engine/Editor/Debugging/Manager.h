#pragma once

#include "Config.h"
#include "ReferenceViewer.h"
#include "Window.h"
#include "Utility/Singelton.h"

namespace Debug
{
    class Manager : public Utility::Singelton<Manager, true>
    {
    public:
        void Init();
        void Deinit();
        void Logic();
        void Frame(double InDeltaTime);
        
        bool Enabled() const { return DebugEnabled; }
        
        void Register(Window* InWindow);
        void Unregister(const Window* InWindow);
        
        bool IsOpen(const String& InWindow) const;
        void SetOpen(const String& InWindow, bool InOpen);

    private:
        bool DebugEnabled = false;
        Set<Window*> PendingRegister;
        Map<String, Vector<Window*>> Windows;
        Map<const Window*, String> WindowToName;
        Config Config;

        ReferenceViewer References;

        // Logic tick counter
        int LogicCounter = 0;
        double TPF = 0.0; 
    };
}
