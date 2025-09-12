#include "Model.h"

#include "raylib.h"
#include "Utility/File/File.h"

bool Rendering::ModelResource::Load()
{
    ptr = new Model(); 
    *ptr = LoadModel(id.Str().c_str());
    cachedHash = Utility::Hash(id.Str());
    return true;
}

bool Rendering::ModelResource::Unload()
{
    if (ptr)
    {
        UnloadModel(*ptr);
        delete ptr;
        ptr = nullptr;
    }
    cachedHash = 0;
    return true; 
}

Model*Rendering::ModelResource::Get() const
{
    if (IsModelValid(*ptr))
        return ptr;
    return nullptr;
}

uint32 Rendering::ModelResource::Hash() const
{
    return cachedHash;
}
