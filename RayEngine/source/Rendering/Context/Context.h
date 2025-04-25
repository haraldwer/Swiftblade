#pragma once

#include "ContextConfig.h"

namespace Rendering
{
    class Lights;
    class Lumin;
    struct LuminConfig;

    class Context
    {
        friend class Pipeline;
        friend class Renderer;
        
    public:
        void Init(const ContextConfig& InConfig, const LuminConfig& InLuminConfig, bool InRoot = false);
        void Deinit();
        double Time() const { return Timer.Ellapsed(); }
        Lights* GetLights() const { return LightsPtr; }

    private:
        ContextConfig Config;

        // Total context lifetime
        Utility::Timer Timer;
        
        Lights* LightsPtr = nullptr;
        Lumin* LuminPtr = nullptr;
    };
}
