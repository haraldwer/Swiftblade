#pragma once
#include "Debug/Panel.h"

namespace Engine
{
    struct LauncherConfig : BaseConfig<LauncherConfig>
    {
        PROPERTY_D(bool, SaveOnExit, true);
        PROPERTY(String, Instance)
        String Name() const override { return "Launcher"; }
    };

    class Launcher : public Debug::Panel
    {
    public:
        void Init();
        void Deinit();

    private:
        void DrawDebugPanel() override;
        String DebugPanelName() const override { return "Launcher"; }

        void Push(const String& InHash);
        
        LauncherConfig config;
    };
}
