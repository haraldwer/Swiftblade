#include "Context.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "State/State.h"

void Rendering::Context::Init(const ContextConfig& InConfig, const LuminConfig& InLuminConfig, const bool InRoot)
{
    config = InConfig;

    // TODO: Use config when compiling shaders
    // Additional arguments for resource constructor?
    
    if (InRoot)
    {
        if (luminPtr)
            luminPtr->Deinit();

        if (config.Lumin)
        {
            if (!luminPtr)
                luminPtr = new Lumin();
            luminPtr->Init(InLuminConfig);
        }

        if (lightsPtr)
            lightsPtr->Deinit();
        else lightsPtr = new Lights();
        lightsPtr->Init(InConfig.Lights);
    }
}

void Rendering::Context::Deinit()
{
    if (luminPtr)
    {
        luminPtr->Deinit();
        delete luminPtr;
        luminPtr = nullptr;
    }
    if (lightsPtr)
    {
        lightsPtr->Deinit();
        delete lightsPtr;
        lightsPtr = nullptr;
    }

    rlState::current.Reset();
    rlState::current.Check();
}
