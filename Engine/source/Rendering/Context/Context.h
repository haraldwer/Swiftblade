#pragma once

#include "ContextConfig.h"

namespace Rendering
{
    class Particles;
    class Lights;
    class Lumin;
    struct LuminConfig;

    class Context
    {
        friend class Pipeline;
        friend class Renderer;
        friend class DeferredRenderer;
        
    public:
        void Init(const ContextConfig& InConfig, bool InRoot = false);
        void Deinit();
        double Time() const { return timer.Ellapsed(); }
        Lights* GetLights() const { return lightsPtr; }
        Lumin* GetLumin() const { return luminPtr; }
        Particles* GetParticles() const { return particlesPtr; }

    private:
        ContextConfig config = {};

        // Total context lifetime
        Utility::Timer timer = {};
        
        Lights* lightsPtr = nullptr;
        Lumin* luminPtr = nullptr;
        Particles* particlesPtr = nullptr;
    };
}
