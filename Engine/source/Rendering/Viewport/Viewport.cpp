#include "Viewport.h"

void Rendering::Viewport::Init(const Vec2I &InSize)
{
    frameTarget.Init(InSize);
}

void Rendering::Viewport::Deinit()
{
    frameTarget.Deinit();
}

void Rendering::Viewport::Resize(const Vec2I &InResolution)
{
    CHECK_RETURN(resolution == InResolution);
    Deinit();
    Init(InResolution);
}
