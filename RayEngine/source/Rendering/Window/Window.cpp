#include "Window.h"

#include "Config.h"
#include "raylib.h"

void Rendering::Window::Open(const WindowConfig& InConfig)
{
    auto getFlags = [](const WindowConfig& InConf)
    {
        unsigned flags = FLAG_WINDOW_RESIZABLE;
        if (InConf.Fullscreen)
            flags |= FLAG_BORDERLESS_WINDOWED_MODE;
        if (InConf.VSync)
            flags |= FLAG_VSYNC_HINT;
        if (InConf.MSAA)
            flags |= FLAG_MSAA_4X_HINT;
    
#ifndef __EMSCRIPTEN__
        flags |= FLAG_WINDOW_ALWAYS_RUN;
#endif
        return flags;
    };
    
    auto prevFlags = getFlags(config);
    
    config = InConfig;

    config.Width = Utility::Math::Max(500, config.Width.Get());
    config.Height = Utility::Math::Max(500, config.Height.Get());
    
    if (!IsWindowReady())
    {
        InitWindow(
            config.Width,
            config.Height,
            config.Title.Get().c_str());
        prevFlags = 0;
    }
    else if (config.Width != GetScreenWidth() || config.Height != GetScreenHeight())
        SetWindowSize( config.Width, config.Height);
    
    auto newFlags = getFlags(config);
    if (prevFlags != newFlags)
    {
        // Clear only changed flags?
        
        ClearWindowState(
            FLAG_VSYNC_HINT |
            FLAG_MSAA_4X_HINT);
        SetWindowState(newFlags);
    }
    
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
#ifndef __EMSCRIPTEN__    
    PROFILE_GL_NAMED("Cap FPS");
    int targetFPS = config.TargetFPS;
    if (targetFPS > 0)
    {
        targetFPS = Utility::Math::Max(targetFPS, 10);
        const double desiredFrameTime = Utility::Math::Max(0.0, (1.0 / targetFPS) - leftoverFrameTime);
        while (frameTimer.Ellapsed() < desiredFrameTime)
        {
        }
        leftoverFrameTime = frameTimer.Ellapsed() - desiredFrameTime;
        frameTimer = {};
    }
#endif
}

bool Rendering::Window::ShouldClose()
{
#ifndef __EMSCRIPTEN__
    return WindowShouldClose();
#else
    return false;
#endif
}

Vec2I Rendering::Window::GetSize()
{
    return { config.Width, config.Height };
}
