#include "Material.h"

#include "Utility/File/File.h"

bool Rendering::MaterialResource::Load()
{
    const bool result = PropertyFile::Load();
    cachedHash = 0;
    cachedDeferredHash = 0;
    return result;
}

Utility::Timepoint Rendering::MaterialResource::GetEditTime() const
{
    Utility::Timepoint maxTime = Utility::Timepoint::min();
    if (const ShaderResource* rsc = data.SurfaceShader.Get().Get())
        maxTime = rsc->GetEditTime();
    if (const ShaderResource* rsc = data.DeferredShader.Get().Get())
        maxTime = Utility::Math::Max(maxTime, rsc->GetEditTime());
    maxTime = Utility::Math::Max(maxTime, PropertyFile::GetEditTime());
    return maxTime;
}

uint32 Rendering::MaterialResource::Hash()
{
    if (cachedHash == 0)
        if (data.SurfaceShader.Get().IsLoaded() && data.DeferredShader.Get().IsLoaded())
            cachedHash =  
                Utility::Hash(data.SurfaceShader.Get().Identifier()) + 
                Utility::Hash(data.TwoSided.Get()) +
                DeferredHash();
    return cachedHash;
}

uint32 Rendering::MaterialResource::DeferredHash()
{
    if (cachedDeferredHash == 0)
        cachedDeferredHash = Utility::Hash(data.DeferredShader.Get().Identifier());
    return cachedDeferredHash;
}
