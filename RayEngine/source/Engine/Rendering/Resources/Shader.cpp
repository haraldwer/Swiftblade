#include "Shader.h"

#include "raylib.h"
#include "Utility/File.h"

bool ShaderResource::Load(const String& InIdentifier)
{
    Ptr = new Shader(); 
    *Ptr = LoadShader(
        ("../Content/" + InIdentifier + ".vs").c_str(),
        ("../Content/" + InIdentifier + ".fs").c_str());
    Identifier = InIdentifier;
    return true;
}

bool ShaderResource::Unload()
{
    if (Ptr)
        UnloadShader(*Ptr); 
    delete Ptr;
    Ptr = nullptr;
    return true;
}

Utility::Timepoint ShaderResource::GetEditTime() const
{
    const Utility::Timepoint vs = Utility::GetFileWriteTime("../Content/" + Identifier + ".vs");
    const Utility::Timepoint fs = Utility::GetFileWriteTime("../Content/" + Identifier + ".fs");
    return MAX(vs, fs); 
}
