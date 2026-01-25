#include "Context.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "Particles/Particles.h"
#include "State/State.h"

void Rendering::Context::Init(const ContextConfig& InConfig, const bool InRoot)
{
    config = InConfig;

    if (InRoot)
    {
        if (luminPtr)
            luminPtr->Deinit();
        else luminPtr = new Lumin();
        luminPtr->Init(config);
        if (lightsPtr)
            lightsPtr->Deinit();
        else lightsPtr = new Lights();
        lightsPtr->Init(config.Lights);
        if (particlesPtr)
            particlesPtr->Deinit();
        else particlesPtr = new Particles();
        particlesPtr->Init(config);
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
}
