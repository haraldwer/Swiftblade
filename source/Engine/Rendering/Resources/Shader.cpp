#include "Shader.h"

bool ShaderResource::Load(const String& InIdentifier)
{
    Ptr = new Shader(); 
    *Ptr = LoadShader(InIdentifier.c_str(), InIdentifier.c_str());
    return true;
}

bool ShaderResource::Unload()
{
    delete Ptr;
    Ptr = nullptr;
    return true;
}
