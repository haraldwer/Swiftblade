#pragma once
#include "Config.h"

struct Texture;

namespace Rendering
{
    class Window
    {
    public:
        void Open(const WindowConfig& InConfig);
        static bool ShouldClose();
        static void Close();
        
        void Draw(const Texture& InTexture);
        void CapFPS();
        
        Vec2I GetSize();

    private:
        WindowConfig config = {};
        Utility::Timer frameTimer = {};
        double leftoverFrameTime = 0;
        
        
    };
}
