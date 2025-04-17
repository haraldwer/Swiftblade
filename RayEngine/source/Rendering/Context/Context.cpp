#include "Context.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"

void Rendering::Context::Init(const ContextConfig& InConfig, const LuminConfig& InLuminConfig, const bool InRoot)
{
    Config = InConfig;

    // TODO: Use config when compiling shaders
    // Additional arguments for resource constructor?
    
    if (InRoot)
    {
        if (LuminPtr)
            LuminPtr->Deinit();
        else LuminPtr = new Lumin();
        LuminPtr->Init(InLuminConfig);

        if (LightsPtr)
            LightsPtr->Deinit();
        else LightsPtr = new Lights();
        LightsPtr->Init(InConfig.Lights);
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
