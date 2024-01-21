#pragma once

namespace Debug
{
    class Window
    {
    public:
        Window();
        virtual ~Window();

        virtual void DrawDebugWindow() = 0;
        virtual String DebugWindowName() const = 0; 
    };
}

