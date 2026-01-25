#pragma once
#include "Debug/Panel.h"

namespace Editor
{
    struct LauncherConfig : BaseConfig<LauncherConfig>
    {
        PROPERTY_D(bool, SaveOnExit, false);
        PROPERTY(String, Instance)
        String Name() const override { return "Launcher"; }
    };

    class Launcher : public Debug::Panel
    {
    public:
        void Init();
        void Deinit();

    private:
        void DrawPanel() override;
        String PanelName() const override { return "Launcher"; }

        void Push(const String& InHash);
        
        LauncherConfig config;
    };
}
