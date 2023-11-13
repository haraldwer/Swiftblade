#include "Material.h"

#include "Utility/File.h"

bool MaterialResource::Load(const String& InPath)
{
    if (!PropertyOwnerBase::Load(InPath))
        return false;
    
    Ptr = new Material();
    *Ptr = LoadMaterialDefault();

    if (const ShaderResource* rsc = Shader.Get().Get())
    {
        const ::Shader& shader = *rsc->Get(); 
        Ptr->shader = shader;
    }

    Identifier = InPath;

    return true;
}

bool MaterialResource::Unload()
{
    if (Ptr)
    {
        Ptr->shader = ::Shader(); // Shader resource managed separately 
        UnloadMaterial(*Ptr);
    }
    delete Ptr;
    Ptr = nullptr;
    return true;
}

Utility::Timepoint MaterialResource::GetEditTime() const
{
    Utility::Timepoint maxTime; 
    if (const ShaderResource* rsc = Shader.Get().Get())
        maxTime = rsc->GetEditTime();
    maxTime = MAX(maxTime, Utility::GetFileWriteTime(Identifier));
    return maxTime;
}
