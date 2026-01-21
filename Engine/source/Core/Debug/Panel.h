#pragma once

namespace Debug
{
    class Panel
    {
    public:
        Panel();
        virtual ~Panel();

        virtual bool PanelBegin(bool& InOutOpen);
        virtual void DrawPanel() = 0;
        virtual void PanelEnd();
        virtual String PanelName() const = 0;
        virtual int PanelPriority() const { return 0; };
        bool IsPanelOpen() const;
    };
}

