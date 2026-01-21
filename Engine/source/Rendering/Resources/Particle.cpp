#include "Particle.h"

bool Rendering::Particle::Load()
{
    if (PropertyFile::Load())
    {
        Bake();
        return true;
    }
    return false;
}

bool Rendering::Particle::Unload()
{
    return PropertyFile::Unload() && BakedExpressions.Unload();
}

bool Rendering::Particle::Edit(const String& InName, uint32 InOffset)
{
    if (PropertyFile::Edit(InName, InOffset))
    {
        Bake();
        return true;
    }
    return false;
}

bool Rendering::Particle::Accept(const String &InPath)
{
    return InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("PS_");
}

void Rendering::Particle::Bake()
{
    data.InitialPosition.Get().Declare(0, "ALPHA");
    data.InitialVelocity.Get().Declare(0, "ALPHA");
    data.Acceleration.Get().Declare(0, "ALPHA");
    data.Friction.Get().Declare(0, "ALPHA");
    data.Color.Get().Declare(0, "ALPHA");
    data.Scale.Get().Declare(0, "ALPHA");
    data.SpawnRate.Get().Declare(0, "ALPHA");
    data.Lifetime.Get().Declare(0, "ALPHA");
    
    data.InitialPosition.Get().Compile();
    data.InitialVelocity.Get().Compile();
    data.Acceleration.Get().Compile();
    data.Friction.Get().Compile();
    data.Color.Get().Compile();
    data.Scale.Get().Compile();
    data.SpawnRate.Get().Compile();
    data.Lifetime.Get().Compile();

    
    BakedExpressions.Bake(data.BakeResolution.Get(),
        [&](const Vec2F& InCoord, const Vec2I& InPixel)
        {
            if (InPixel.x == 0) return Eval(data.InitialPosition.Get(), InCoord.y);
            if (InPixel.x == 1) return Eval(data.InitialVelocity.Get(), InCoord.y);
            if (InPixel.x == 2) return Eval(data.Acceleration.Get(), InCoord.y);
            if (InPixel.x == 3) return Eval(data.Friction.Get(), InCoord.y);
            if (InPixel.x == 4) return Eval(data.Color.Get(), InCoord.y);
            if (InPixel.x == 5) return Eval(data.Scale.Get(), data.SpawnRate.Get(), data.Lifetime.Get(), InCoord.y);
            return Vec4F();
        });
}
