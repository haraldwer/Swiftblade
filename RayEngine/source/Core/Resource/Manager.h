#pragma once

#include "Core/Debug/Panel.h"

namespace Resource
{
    struct Base;
    
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Panel
    {
    public:

        Base* GetResource(const String& InIdentifier);
        void Register(Base* InResource, const String& InIdentifier);
        
        void Update();
        void Deinit();

        void DrawDebugPanel() override;
        String DebugPanelName() const override { return "Resources"; }
        
    private:
        void TryUnload() const;
        
        Map<String, Base*> Resources = {};
        Utility::Timer CheckTimer = {};
        Vector<String> HotReloadQueue = {};
        
        static constexpr float CheckInterval = 0.05f; 
        static constexpr int CheckNum = 1; 
    };
}
