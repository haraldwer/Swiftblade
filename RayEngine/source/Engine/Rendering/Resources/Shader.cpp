#include "Shader.h"

#include "Utility/File/File.h"
#include "raylib.h"
#include "rlgl.h"

bool ShaderResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;

    Ptr = new Shader();

    // TODO: Load individual shaders
    // Compile shader variations
    
    if (InIdentifier.ends_with(".ds") || InIdentifier.ends_with(".ps"))
    {
        // Load as deferred shader
        if (!Utility::FileExists(InIdentifier))
            return false; 
        const String vsFile = "Shaders/SH_Fullscreen.vs";
        const String vsCode = LoadShaderFile(vsFile);
        const String fsCode = LoadShaderFile(InIdentifier);
        LOG("Compiling shader fs: " +  InIdentifier + " vs: " + vsFile)
        *Ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
        if (!IsShaderValid(*Ptr))
            return false;
    }
    else
    {
        const String vs = InIdentifier + ".vs";
        const String fs = InIdentifier + ".fs";
        const String vsDefault = "Shaders/SH_Default.vs";
        const String fsDefault = "Shaders/SH_Default.fs";
        const String vsFile = Utility::FileExists(vs) ? vs : vsDefault;
        const String fsFile = Utility::FileExists(fs) ? fs : fsDefault;
        const String vsCode = LoadShaderFile(vsFile);
        const String fsCode = LoadShaderFile(fsFile);
        LOG("Compiling shader fs: " +  fsFile + " vs: " + vsFile)
        *Ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
        if (!IsShaderValid(*Ptr))
            return false;
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
    Includes.clear();
    Locations.clear();
    return true;
}

Utility::Timepoint ShaderResource::GetEditTime() const
{
    Utility::Timepoint max = Utility::Timepoint::min(); 
    for (String path : Includes)
        max = Utility::Math::Max(max, Utility::GetFileWriteTime(path));
    return  max;
}

Shader* ShaderResource::Get() const
{
    if (Ptr && IsShaderValid(*Ptr))
        return Ptr;
    if (Identifier.ends_with(".ds"))
    {
        if (const auto defaultShader = ResShader("Shaders/SH_Default.ds").Get())
            return defaultShader->Get();
    }
    else if (Identifier.ends_with(".ps"))
    {
        return nullptr;
    }
    else
    {
        if (const auto defaultShader = ResShader("Shaders/SH_Default").Get())
            return defaultShader->Get();
    }
    return nullptr; 
}

int ShaderResource::GetLocation(const String& InValue)
{
    const Shader* ptr = Get();
    CHECK_RETURN(!ptr, -1);
    const auto find = Locations.find(InValue);
    if (find != Locations.end())
        return find->second;
    const int loc = rlGetLocationUniform(Ptr->id, InValue.c_str());
    //if (loc < 0)
    //    LOG("Failed to find texture location: " + InValue);
    Locations[InValue] = loc; 
    return loc; 
}

String ShaderResource::LoadShaderFile(const String& InPath)
{
    Includes.clear(); 
    String shader = Utility::ReadFile(InPath);
    shader = ProcessIncludes(shader, InPath);
    // TODO: Defines
    return shader;
}

String ShaderResource::ProcessIncludes(const String& InShaderCode, const String& InPath)
{
    Includes.insert(InPath);
    
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

        if (Utility::FileExists(includePath))
        {
            if (!Includes.contains(includePath))
            {
                const String includeContent = Utility::ReadFile(includePath);
                const String processedInclude = ProcessIncludes(includeContent, includePath);
                processedShader += processedInclude; 
            }
        }
        else
        {
            LOG("Tried to include non-existing file: " + includePath + " in " + InPath);
        }
        
        searchOffset = includeEnd + 1;
    }
    return processedShader;
}
