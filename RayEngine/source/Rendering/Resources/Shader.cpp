#include "Shader.h"

#include <functional>

#include "Utility/File/File.h"
#include "raylib.h"
#include "rlgl.h"
#include "Engine/Rendering/Manager.h"

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
        const String vsCode = LoadShaderFile(vsFile, VSIncludes);
        const String fsCode = LoadShaderFile(InIdentifier, FSIncludes);
        *Ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
        CHECK_RETURN_LOG(!IsShaderValid(*Ptr), "Compile failed. fs: " +  InIdentifier + " vs: " + vsFile, true);
    }
    else
    {
        String id = InIdentifier;
        if (id.ends_with(".fs") || id.ends_with(".vs"))
            id = id.substr(0, id.length() - 3);
        const String vs = id + ".vs";
        const String fs = id + ".fs";
        const String vsDefault = "Shaders/SH_Default.vs";
        const String fsDefault = "Shaders/SH_Default.fs";
        const String vsFile = Utility::FileExists(vs) ? vs : vsDefault;
        const String fsFile = Utility::FileExists(fs) ? fs : fsDefault;
        const String vsCode = LoadShaderFile(vsFile, VSIncludes);
        const String fsCode = LoadShaderFile(fsFile, FSIncludes);
        *Ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
        CHECK_RETURN_LOG(!IsShaderValid(*Ptr), "Compile failed. fs: " +  fsFile + " vs: " + vsFile, true);
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
    FSIncludes.clear();
    VSIncludes.clear();
    Locations.clear();
    return true;
}

Utility::Timepoint ShaderResource::GetEditTime() const
{
    if (Defines != Rendering::Manager::Get().GetConfig().Context.Get().GlobalDefines.Get())
        return std::chrono::high_resolution_clock::now();
    
    Utility::Timepoint max = Utility::Timepoint::min(); 
    for (String path : FSIncludes)
        max = Utility::Math::Max(max, Utility::GetFileWriteTime(path));
    for (String path : VSIncludes)
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
            if (defaultShader != this)
                return defaultShader->Get();
    }
    else if (Identifier.ends_with(".ps"))
    {
        return nullptr;
    }
    else
    {
        if (const auto defaultShader = ResShader("Shaders/SH_Default").Get())
            if (defaultShader != this)
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

String ShaderResource::LoadShaderFile(const String& InPath, Set<String>& InIncludes)
{
    InIncludes.clear();
    String shader = Utility::ReadFile(InPath);
    shader = ProcessDefines(shader);
    shader = ProcessIncludes(shader, InPath, InIncludes);
    return shader;
}

String ShaderResource::ProcessIncludes(const String& InShaderCode, const String& InPath, Set<String>& InIncludes)
{
    InIncludes.insert(InPath);
    
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
            if (!InIncludes.contains(includePath))
            {
                const String includeContent = Utility::ReadFile(includePath);
                const String processedInclude = ProcessIncludes(includeContent, includePath, InIncludes);
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

String ShaderResource::ProcessDefines(const String& InShaderCode)
{
    std::function search = [&](String& InCode, const String& InSearchStart, const String& InSearchEnd, const std::function<String(const String& InContent)>& InReplaceFunc) -> bool
    {
        bool found = false;
        String processedShader;
        size_t searchOffset = 0;
        while (true)
        {
            const size_t findOffset = InCode.find(InSearchStart, searchOffset);
            processedShader += InCode.substr(searchOffset, findOffset - searchOffset); 
            if (findOffset == std::string::npos)
                break;
            
            const size_t findEnd = InCode.find_first_of(InSearchEnd, findOffset + InSearchStart.length());
            const size_t findStart = findOffset + InSearchStart.length();
            const String findStr = InCode.substr(findStart, findEnd - findStart);
            processedShader += InReplaceFunc(findStr);
            searchOffset = findEnd + InSearchEnd.length();
            found = true;
        }
        InCode = processedShader;
        return found; 
    };
    
    String result = InShaderCode;
    auto c = Rendering::Manager::Get().GetConfig();
    Defines = c.Context.Get().GlobalDefines;

    // Offset for #version
    auto verStart = result.find_first_of("#version");
    auto verEnd = result.find_first_of('\n', verStart);
    for (auto& def : c.Context.Get().GlobalDefines.Get())
        if (!def.empty())
            result.insert(verEnd + 1, "\n#define " + def + "\n");

    return result;
    
    // Find defines
    Set<String> defines;
    std::function foundDefine = [&](const String& InFindStr) -> String
    {
        auto start = InFindStr.find_first_not_of(' ');
        auto end = InFindStr.find_last_not_of(' ');
        defines.insert(InFindStr.substr(start, end - start));
        return "";
    };

    // ifdefs
    std::function foundIfdef = [&](const String& InFindStr) -> String
    {
        String str = InFindStr;
        bool negated = InFindStr.starts_with("ndef");
        if (negated)
            str = str.substr(4);
        
        auto defEnd = str.find_first_of('\n');
        auto defStart = str.find_first_not_of(' ');
        String def = str.substr(defStart, defEnd - defStart);
        
        if (defines.contains(def) != negated)
            return str.substr(defEnd);
        return ""; 
    };
    
    while (true)
    {
        bool changed = false;
        changed |= search(result, "#if", "#endif", foundIfdef); // Resolve ifdefs
        changed |= search(result, "#define ", "\n", foundDefine); // Resolve defines
        if (!changed)
            break;
    }

    return result;
}
