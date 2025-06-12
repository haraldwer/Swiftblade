#pragma once

#include "Context/ContextConfig.h"
#include "Lumin/LuminConfig.h"
#include "Viewport/ViewportConfig.h"
#include "Window/WindowConfig.h"

namespace Rendering
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY(WindowConfig, Window);
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY(ContextConfig, Context);
        PROPERTY(LuminConfig, Lumin);
        
        PROPERTY_D(bool, UpdateCulling, true);

        String Name() const override { return "Rendering"; }
    };
}
