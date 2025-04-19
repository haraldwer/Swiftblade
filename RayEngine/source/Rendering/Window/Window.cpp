#include "Window.h"

#include "Config.h"
#include "raylib.h"

void Rendering::Window::Open(const WindowConfig& InConfig)
{
    Config = InConfig;

    if (!IsWindowReady())
    {
        InitWindow(
            InConfig.Width,
            InConfig.Height,
            "RayEngine");
    }
    else
    {
        SetWindowSize(
            InConfig.Width,
            InConfig.Height);
    }

    unsigned flags = 0;
    if (InConfig.Fullscreen)
        flags |= FLAG_BORDERLESS_WINDOWED_MODE;
    if (InConfig.VSync)
        flags |= FLAG_VSYNC_HINT;
    if (InConfig.MSAA)
        flags |= FLAG_MSAA_4X_HINT;
    flags |= FLAG_WINDOW_ALWAYS_RUN;
    SetWindowState(flags);
    
    SetExitKey(KEY_F4);
}

void Rendering::Window::Close()
{
    if (IsWindowReady())
        CloseWindow();
}

void Rendering::Window::Draw(const Texture& InTexture)
{    
    // Flip and blip
    const float virtualRatio = static_cast<float>(Config.Width) / static_cast<float>(Config.Height);
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(InTexture.width),
        -static_cast<float>(InTexture.height)
    };
    const Rectangle destRec = {
        -virtualRatio,
        -virtualRatio,
        static_cast<float>(Config.Width) + (virtualRatio * 2),
        static_cast<float>(Config.Height) + (virtualRatio * 2)
    };
    DrawTexturePro(
        InTexture,
        sourceRec,
        destRec,
        {0.0, 0.0},
        0.0,
        ::WHITE);
}

void Rendering::Window::CapFPS()
{
    int targetFPS = Config.TargetFPS;
    if (targetFPS > 0)
    {
        targetFPS = Utility::Math::Max(targetFPS, 10);
        double desiredFrameTime = (1.0 / targetFPS) - LeftoverFrameTime;
        while (FrameTimer.Ellapsed() < desiredFrameTime)
        {
        }
        LeftoverFrameTime = FrameTimer.Ellapsed() - desiredFrameTime;
        FrameTimer = {};
    }
}

bool Rendering::Window::ShouldClose() const
{
    return WindowShouldClose();
}

Vec2I Rendering::Window::GetSize()
{
    return { Config.Width, Config.Height };
}
