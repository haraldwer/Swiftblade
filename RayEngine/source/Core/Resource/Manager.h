#pragma once

#include "Identifier.h"
#include "Core/Debug/Panel.h"

namespace Resource
{
    struct ImplBase;
    
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Panel
    {
    public:

        ImplBase* GetResource(const ID& InID);
        void Register(ImplBase* InResource, const ID& InID);
        
        void Update();
        void Deinit();

        void DrawDebugPanel() override;
        String DebugPanelName() const override { return "Resources"; }
        
    private:
        void TryUnload() const;
        
        Map<uint32, ImplBase*> resources = {};
        Utility::Timer checkTimer = {};
        Vector<uint32> hotReloadQueue = {};
        uint32 selected = -1;
        
        static constexpr float CHECK_INTERVAL = 0.05f; 
        static constexpr int CHECK_NUM = 1; 
    };
}
