#include "Material.h"

#include "Utility/File/File.h"

bool MaterialResource::Load(const String& InPath)
{
    identifier = InPath;
    const bool result = PropertyOwnerBase::Load(InPath);
    cachedHash = 0;
    cachedDeferredHash = 0;
    return result;
}

Utility::Timepoint MaterialResource::GetEditTime() const
{
    Utility::Timepoint maxTime = Utility::Timepoint::min();
    if (const ShaderResource* rsc = SurfaceShader.Get().Get())
        maxTime = rsc->GetEditTime();
    if (const ShaderResource* rsc = DeferredShader.Get().Get())
        maxTime = Utility::Math::Max(maxTime, rsc->GetEditTime());
    maxTime = Utility::Math::Max(maxTime, Utility::GetFileWriteTime(identifier));
    return maxTime;
}

uint32 MaterialResource::Hash()
{
    if (cachedHash == 0)
        if (SurfaceShader.Get().IsLoaded() && DeferredShader.Get().IsLoaded())
            cachedHash =  
                Utility::Hash(SurfaceShader.Get().Identifier()) + 
                Utility::Hash(TwoSided.Get()) +
                DeferredHash();
    return cachedHash;
}

uint32 MaterialResource::DeferredHash()
{
    if (cachedDeferredHash == 0)
        cachedDeferredHash = Utility::Hash(DeferredShader.Get().Identifier());
    return cachedDeferredHash;
}
