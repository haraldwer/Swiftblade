#include "Model.h"

#include "raylib.h"

bool ModelResource::Load(const String& InIdentifier)
{
    Ptr = new Model(); 
    *Ptr = LoadModel(InIdentifier.c_str());
    return true;
}

bool ModelResource::Unload()
{
    return true; 
}
