#include "Material.h"

#include "Utility/File/File.h"

bool MaterialResource::Load(const String& InPath)
{
    Identifier = InPath;
    return PropertyOwnerBase::Load(InPath);
}

Utility::Timepoint MaterialResource::GetEditTime() const
{
    Utility::Timepoint maxTime; 
    if (const ShaderResource* rsc = Shader.Get().Get())
        maxTime = rsc->GetEditTime();
    maxTime = MAX(maxTime, Utility::GetFileWriteTime(Identifier));
    return maxTime;
}
