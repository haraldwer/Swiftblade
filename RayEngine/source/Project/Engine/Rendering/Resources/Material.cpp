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
        maxTime = MAX(maxTime, rsc->GetEditTime());
    maxTime = MAX(maxTime, Utility::GetFileWriteTime(Identifier));
    return maxTime;
}

uint32 MaterialResource::SurfaceHash() const
{
}

uint32 MaterialResource::DeferredHash() const
{
}
