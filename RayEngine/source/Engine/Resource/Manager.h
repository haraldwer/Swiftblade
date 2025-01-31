#pragma once

#include "Editor/Debugging/Window.h"

namespace Resource
{
    struct Base;
    
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Window
    {
    public:

        Base* GetResource(const String& InIdentifier);
        void Register(Base* InResource, const String& InIdentifier);
        
        void Update();
        void HotReload(int InNum) const;
        void Deinit();

        void DrawDebugWindow() override;
        String DebugWindowName() const override { return "Resources"; }
        
    private:
        Map<String, Base*> Resources;
        Utility::Timer CheckTimer;
        static constexpr float CheckInterval = 1.0f; 
    };
}
