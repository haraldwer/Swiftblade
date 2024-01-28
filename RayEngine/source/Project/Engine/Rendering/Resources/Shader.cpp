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

    // TODO: Load individual shaders
    // Compile shader variations

    const String vsCode = LoadShaderFile((Utility::FileExists(vs) ? vs : vsDefault));
    const String fsCode = LoadShaderFile((Utility::FileExists(fs) ? fs : fsDefault));
    *Ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
    //Ptr->id = rlLoadShaderCode(vsCode.c_str(), fsCode.c_str());
    
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
        rlUnloadShaderProgram(Ptr->id); 
    delete Ptr;
    Ptr = nullptr;
    IncludeGuard.clear(); 
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

String ShaderResource::LoadShaderFile(const String& InPath)
{
    IncludeGuard.clear(); 
    String shader = Utility::ReadFile(InPath);
    shader = ProcessIncludes(shader, InPath);
    // TODO: Defines
    return shader;
}

String ShaderResource::ProcessIncludes(const String& InShaderCode, const String& InPath)
{
    IncludeGuard.insert(InPath);
    
    String processedShader;
    size_t searchOffset = 0;
    const String searchPattern = "#include \"";
    while (true)
    {
        const size_t findOffset = InShaderCode.find(searchPattern, searchOffset);
        processedShader += InShaderCode.substr(searchOffset, findOffset - searchOffset); 
        if (findOffset == std::string::npos)
            break;

        const size_t includeEnd = InShaderCode.find_first_of('\"', findOffset + searchPattern.length());
        const size_t includeStart = findOffset + searchPattern.length();
        const String includePath = InShaderCode.substr(includeStart, includeEnd - includeStart);
        if (!IncludeGuard.contains(includePath))
        {
            LOG("Including shader " + includePath + " in " + InPath);
            const String includeContent = Utility::ReadFile(includePath);
            const String processedInclude = ProcessIncludes(includeContent, includePath); 
            processedShader += processedInclude; 
        }
        searchOffset = includeEnd + 1;
    }
    LOG("Shader processed: " + InPath); 
    return processedShader;
}
