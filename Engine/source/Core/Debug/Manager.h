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
        void Deinit() const;
        void Logic();
        void Frame(double InDeltaTime);
        
        bool Enabled() const { return current.DebugEnabled; }
        
        void Register(Panel* InWindow);
        void Unregister(const Panel* InWindow);
        
        bool IsOpen(const String& InWindow) const;
        void SetOpen(const String& InWindow, bool InOpen);

    private:
        Set<Panel*> pendingRegister = {};
        
        OrderedMap<int, OrderedMap<String, Vector<Panel*>>> panels;
        Map<const Panel*, String> panelToName;
        Map<const Panel*, int> panelToPriority;
        
        Config current = {};


        // Logic tick counter
        int logicCounter = 0;
        double tpf = 0.0;
        double fps = 0.0;
        double tps = 0.0;
    };
}
