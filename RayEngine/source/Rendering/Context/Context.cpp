#include "Context.h"

#include "Lumin/Lumin.h"

void Rendering::Context::Init(const ContextConfig& InConfig, const bool InLuminInstance)
{
    Config = InConfig;

    // TODO: Use config when compiling shaders
    // Additional arguments for resource constructor?
    
    SSAOShader = ResShader("Shaders/PostProcessing/SH_SSAO.ps"); 
    QuantizeShader = ResShader("Shaders/PostProcessing/SH_Quantize.ps");
    FXAAShader = ResShader("Shaders/PostProcessing/SH_FXAA.ps");
    
    FireShader = ResShader("Shaders/PostProcessing/SH_Fire.ps");
    FireBlipShader = ResShader("Shaders/PostProcessing/SH_FireBlip.ps");

    if (InLuminInstance && InConfig.Lumin)
    {
        LuminPtr = new Lumin();
        LuminPtr->Init(InConfig);
    }
}

void Rendering::Context::Deinit()
{
    if (LuminPtr)
    {
        LuminPtr->Deinit();
        delete LuminPtr;
        LuminPtr = nullptr;
    }
}
