#include "Model.h"

#include "raylib.h"
#include "Utility/File/File.h"

bool ModelResource::Load(const String& InIdentifier)
{
    ptr = new Model(); 
    *ptr = LoadModel(InIdentifier.c_str());
    identifier = InIdentifier;
    cachedHash = Utility::Hash(identifier);
    return true;
}

bool ModelResource::Unload()
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

Utility::Timepoint ModelResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);
}

Model* ModelResource::Get() const
{
    if (IsModelValid(*ptr))
        return ptr;
    return nullptr;
}

uint32 ModelResource::Hash() const
{
    return cachedHash;
}
