#include "Context.h"

void Rendering::Context::Init(const ContextConfig& InConfig)
{
    Config = InConfig;

    // TODO: Use config when compiling shaders
    // Additional arguments for resource constructor?
    
    SSAOShader = ResShader("Shaders/PostProcessing/SH_SSAO.ps"); 
    QuantizeShader = ResShader("Shaders/PostProcessing/SH_Quantize.ps");
    FXAAShader = ResShader("Shaders/PostProcessing/SH_FXAA.ps");
    
    FireShader = ResShader("Shaders/PostProcessing/SH_Fire.ps");
    FireBlipShader = ResShader("Shaders/PostProcessing/SH_FireBlip.ps");
}
