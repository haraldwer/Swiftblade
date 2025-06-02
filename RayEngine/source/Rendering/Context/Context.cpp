#include "Context.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "State/State.h"

void Rendering::Context::Init(const ContextConfig& InConfig, const LuminConfig& InLuminConfig, const bool InRoot)
{
    Config = InConfig;

    // TODO: Use config when compiling shaders
    // Additional arguments for resource constructor?
    
    if (InRoot)
    {
        if (LuminPtr)
            LuminPtr->Deinit();

        if (Config.Lumin)
        {
            if (!LuminPtr)
                LuminPtr = new Lumin();
            LuminPtr->Init(InLuminConfig);
        }

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
    if (LightsPtr)
    {
        LightsPtr->Deinit();
        delete LightsPtr;
        LightsPtr = nullptr;
    }

    rlState::current.Reset();
    rlState::current.Check();
}
