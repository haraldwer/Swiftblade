#include "Material.h"

#include "Utility/File/File.h"

bool MaterialResource::Load(const String& InPath)
{
    Identifier = InPath;
    return PropertyOwnerBase::Load(InPath);
}

Utility::Timepoint MaterialResource::GetEditTime() const
{
    Utility::Timepoint maxTime = Utility::Timepoint::min();
    if (const ShaderResource* rsc = SurfaceShader.Get().Get())
        maxTime = rsc->GetEditTime();
    if (const ShaderResource* rsc = DeferredShader.Get().Get())
        maxTime = Utility::Math::Max(maxTime, rsc->GetEditTime());
    maxTime = Utility::Math::Max(maxTime, Utility::GetFileWriteTime(Identifier));
    return maxTime;
}

uint32 MaterialResource::SurfaceHash() const
{
    return 
        Utility::Hash(SurfaceShader.Get().Identifier()) +
        Utility::Hash(TwoSided.Get());
    // TODO: Material properties
}

uint32 MaterialResource::DeferredHash() const
{
    return Utility::Hash(DeferredShader.Get().Identifier());
}
