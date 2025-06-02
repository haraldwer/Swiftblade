#pragma once

namespace Debug
{
    class Panel
    {
    public:
        Panel();
        virtual ~Panel();

        virtual void DrawDebugPanel() = 0;
        virtual String DebugPanelName() const = 0;
        bool IsDebugPanelOpen() const;
    };
}

