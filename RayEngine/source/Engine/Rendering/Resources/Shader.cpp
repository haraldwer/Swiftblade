#include "Shader.h"

#include "raylib.h"
#include "Utility/File.h"

bool ShaderResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    
    Ptr = new Shader(); 
    *Ptr = LoadShader(
        ("../Content/" + InIdentifier + ".vs").c_str(),
        ("../Content/" + InIdentifier + ".fs").c_str());

    if (!IsShaderReady(*Ptr))
        return false;
    
    Ptr->locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(*Ptr, "mvp");
    Ptr->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(*Ptr, "instanceTransform");
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

Shader* ShaderResource::Get() const
{
    if (Ptr && IsShaderReady(*Ptr))
        return Ptr;
    return nullptr; 
}
