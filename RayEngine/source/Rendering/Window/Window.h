#pragma once
#include "Config.h"

namespace Rendering
{
    class Window
    {
    public:
        void Open(const WindowConfig& InConfig);
        bool ShouldClose() const;
        void Close();
        
        void Draw(const Texture& InTexture);
        void CapFPS();
        
        Vec2I GetSize();

    private:
        WindowConfig Config;
        Utility::Timer FrameTimer;
        double LeftoverFrameTime = 0;
        
    };
}
