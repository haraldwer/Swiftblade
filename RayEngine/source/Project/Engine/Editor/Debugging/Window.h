#pragma once

namespace Debug
{
    class Window
    {
    public:
        Window();
        virtual ~Window();

        virtual void DrawDebugWindow() = 0;

        bool IsDebugUIOpen() const { return Open; };
        void ToggleDebugUI() { Open = !Open; }

    private:
        bool Open = true;
    };
}

