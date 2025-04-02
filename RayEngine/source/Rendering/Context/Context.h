#pragma once

#include "ContextConfig.h"
#include "Rendering/Resources/Shader.h"

namespace Rendering
{
    class Lumin;

    class Context
    {
        friend class Pipeline;
        friend class Renderer;
        
    public:
        void Init(const ContextConfig& InConfig, bool InLuminInstance = false);
        void Deinit();

    private:
        ContextConfig Config;

        // Total context lifetime
        Utility::Timer Timer;
        
        ResShader FireShader;
        ResShader FireBlipShader;
        ResShader SSAOShader; 
        ResShader QuantizeShader;
        ResShader FXAAShader;

        Lumin* LuminPtr = nullptr;
    };
}
