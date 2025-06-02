#include "Window.h"

#include "Config.h"
#include "raylib.h"

void Rendering::Window::Open(const WindowConfig& InConfig)
{
    config = InConfig;

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
    const float virtualRatio = static_cast<float>(config.Width) / static_cast<float>(config.Height);
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(InTexture.width),
        -static_cast<float>(InTexture.height)
    };
    const Rectangle destRec = {
        -virtualRatio,
        -virtualRatio,
        static_cast<float>(config.Width) + (virtualRatio * 2),
        static_cast<float>(config.Height) + (virtualRatio * 2)
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
    PROFILE_GL_NAMED("Cap FPS");
    int targetFPS = config.TargetFPS;
    if (targetFPS > 0)
    {
        targetFPS = Utility::Math::Max(targetFPS, 10);
        const double desiredFrameTime = (1.0 / targetFPS) - leftoverFrameTime;
        while (frameTimer.Ellapsed() < desiredFrameTime)
        {
        }
        leftoverFrameTime = frameTimer.Ellapsed() - desiredFrameTime;
        frameTimer = {};
    }
}

bool Rendering::Window::ShouldClose()
{
    return WindowShouldClose();
}

Vec2I Rendering::Window::GetSize()
{
    return { config.Width, config.Height };
}
