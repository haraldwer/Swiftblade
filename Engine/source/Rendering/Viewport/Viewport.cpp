#include "Viewport.h"

void Rendering::Viewport::Init(const Vec2I &InSize)
{
    size = InSize;
    targets.Init(size);
}

void Rendering::Viewport::Deinit()
{
    targets.Deinit();
    size = {};
}

void Rendering::Viewport::Resize(const Vec2I &InSize)
{
    CHECK_RETURN(size == InSize);
    Deinit();
    Init(size);
}
