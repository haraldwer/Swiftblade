#include "Model.h"

#include "raylib.h"
#include "Utility/File/File.h"

bool ModelResource::Load(const String& InIdentifier)
{
    Ptr = new Model(); 
    *Ptr = LoadModel(("../content/" + InIdentifier).c_str());
    Identifier = InIdentifier;
    return true;
}

bool ModelResource::Unload()
{
    if (Ptr)
        UnloadModel(*Ptr);
    delete Ptr;
    Ptr = nullptr; 
    return true; 
}

Utility::Timepoint ModelResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);
}

uint32 ModelResource::Hash() const
{
    return Utility::Hash(Identifier);
}
