#pragma once

#include "Config.h"
#include "Panel.h"
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
        
        bool Enabled() const { return Current.DebugEnabled; }
        
        void Register(Panel* InWindow);
        void Unregister(const Panel* InWindow);
        
        bool IsOpen(const String& InWindow) const;
        void SetOpen(const String& InWindow, bool InOpen);

    private:
        Set<Panel*> PendingRegister = {};
        Map<String, Vector<Panel*>> Windows = {};
        Map<const Panel*, String> WindowToName = {};
        Config Current = {};

        // Logic tick counter
        int LogicCounter = 0;
        double TPF = 0.0; 
    };
}
