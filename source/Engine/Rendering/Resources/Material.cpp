#include "Material.h"

bool MaterialResource::Load(const String& InPath)
{
    if (!PropertyOwner::Load(InPath))
        return false;
    
    Ptr = new Material();
    *Ptr = LoadMaterialDefault();

    if (const ShaderResource* rsc = Shader.Get().Get())
        Ptr->shader = *rsc->Get();

    return true;
}

bool MaterialResource::Unload()
{
    delete Ptr;
    Ptr = nullptr;
    return true;
}
