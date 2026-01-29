#pragma once
#include "Manager.h"
#include "Commands/Command.h"

namespace Rendering
{
    class Context;
    class Window;
    class RenderTarget;
    class CommandList;

    // Not a namespace = limits access using friends keyword
    class ImGuiContext
    {
public:
        static void Init(Window& InWindow, Context& InContext);
        static void Deinit();
        static void BeginFrame();
        static void EndFrame();

        static Command Command(RenderTarget& InTarget);
    };
}
