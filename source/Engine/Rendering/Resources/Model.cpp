#include "Model.h"

bool ModelResource::Load(const String& InIdentifier)
{
    Ptr = new Model(); 
    *Ptr = LoadModel(("../content/" + InIdentifier).c_str());
    return true;
}

bool ModelResource::Unload()
{
    return true; 
}
