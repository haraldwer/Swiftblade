#pragma once
#include "Context/ContextConfig.h"
#include "Window/WindowConfig.h"
#include "Viewport/ViewportConfig.h"

namespace Rendering
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY(WindowConfig, Window);
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY(ContextConfig, Context);

        String Name() const override { return "Rendering"; }
    };
}
