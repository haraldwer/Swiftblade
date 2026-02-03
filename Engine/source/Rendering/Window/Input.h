#pragma once

#include "Utility/Singelton.h"

typedef void* WindowHandle;

namespace Rendering
{
    class Input : public Utility::Singelton<Input, true>
    {
        friend class Window;
    public:
        bool GetKey(int InKey) const;
        bool GetMouseButton(int InButton) const;
        bool GetGamepadButton(int InButton) const;
        float GetGamepadAxis(int InAxis) const;
        Vec2F GetMouseWheel() const;
        Vec2D GetMousePosition() const; 
        Vec2D GetMouseDelta() const;
        
        bool IsCursorHidden() const;
        void HideCursor() const;
        void ShowCursor() const;
    
        void OnCursorPosUpdated(WindowHandle window, double xpos, double ypos);
        
    private:
        void Deinit();
        void Init(WindowHandle InWindow);
        void Frame();
        
        WindowHandle window;
        Vec2D mouseDelta = {};
        Vec2D mousePosition = {};
    };
}