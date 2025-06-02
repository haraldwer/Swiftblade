#include "Material.h"

#include "Utility/File/File.h"

bool MaterialResource::Load(const String& InPath)
{
    Identifier = InPath;
    const bool result = PropertyOwnerBase::Load(InPath);
    CachedHash = 0;
    CachedDeferredHash = 0;
    return result;
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

uint32 MaterialResource::Hash()
{
    if (CachedHash == 0)
        if (SurfaceShader.Get().IsLoaded() && DeferredShader.Get().IsLoaded())
            CachedHash =  
                Utility::Hash(SurfaceShader.Get().Identifier()) + 
                Utility::Hash(TwoSided.Get()) +
                DeferredHash();
    return CachedHash;
}

uint32 MaterialResource::DeferredHash()
{
    if (CachedDeferredHash == 0)
        CachedDeferredHash = Utility::Hash(DeferredShader.Get().Identifier());
    return CachedDeferredHash;
}
