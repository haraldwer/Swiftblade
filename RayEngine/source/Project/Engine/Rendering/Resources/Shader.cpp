#include "Shader.h"

#include "raylib.h"
#include "rlgl.h"
#include "Utility/File/File.h"

bool ShaderResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    
    Ptr = new Shader();

    const String vs = InIdentifier + ".vs";
    const String vsDefault = "Defaults/SH_Default.vs";
    const String fs = InIdentifier + ".fs";
    const String fsDefault = "Defaults/SH_Default.fs";
    
    *Ptr = LoadShader(
        (Utility::FileExists(vs) ? vs : vsDefault).c_str(),
        (Utility::FileExists(fs) ? fs : fsDefault).c_str());

    if (IsShaderReady(*Ptr))
    {
        Ptr->locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(*Ptr, "mvp");
        Ptr->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(*Ptr, "instanceTransform");
    }

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
    const Utility::Timepoint vs = Utility::GetFileWriteTime(Identifier + ".vs");
    const Utility::Timepoint fs = Utility::GetFileWriteTime(Identifier + ".fs");
    if (vs == Utility::Timepoint())
        return fs;
    if (fs == Utility::Timepoint())
        return vs;
    return MAX(vs, fs); 
}

Shader* ShaderResource::Get() const
{
    if (Ptr && IsShaderReady(*Ptr))
        return Ptr;
    if (const auto defaultShader = ResShader("Default/SH_Default").Get())
        return defaultShader->Get();
    return nullptr; 
}
