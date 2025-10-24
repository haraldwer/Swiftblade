#pragma once

#include "Context/ContextConfig.h"
#include "Viewport/ViewportConfig.h"
#include "Window/WindowConfig.h"

namespace Rendering
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY(WindowConfig, Window)
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY(ContextConfig, Context);
        
        PROPERTY_D(bool, UpdateCulling, true);
        PROPERTY_D(bool, DrawElementRects, false);
        PROPERTY_D(bool, DrawPhysicsWorld, false);
        PROPERTY_D(bool, DrawUI, true);
        PROPERTY_D(bool, ShowLumin, false);
        PROPERTY_D(bool, ShowLights, false);

        String Name() const override { return "Rendering"; }
    };
}
