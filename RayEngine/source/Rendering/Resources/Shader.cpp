#include "Shader.h"

#include <functional>

#include "Engine/Rendering/Manager.h"
#include "Utility/File/File.h"
#include "raylib.h"
#include "rlgl.h"

ShaderResource::ShaderResource()
{
    for (auto& l : defaultLocs)
        l = -1;
}

bool ShaderResource::Load(const String& InIdentifier)
{
    PROFILE_GL();
    
    identifier = InIdentifier;
    ptr = new Shader();
    
    // TODO: Load individual shaders
    // Compile shader variations
    
    if (InIdentifier.ends_with(".ds") || InIdentifier.ends_with(".ps"))
    {
        // Load as deferred shader
        if (!Utility::FileExists(InIdentifier))
            return false; 
        const String vsFile = "Shaders/SH_Fullscreen.vs";
        const String vsCode = LoadShaderFile(vsFile, vsIncludes);
        const String fsCode = LoadShaderFile(InIdentifier, fsIncludes);
        *ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
        CHECK_RETURN_LOG(!IsShaderValid(*ptr), "Compile failed. fs: " +  InIdentifier + " vs: " + vsFile, true);
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
        const String vsCode = LoadShaderFile(vsFile, vsIncludes);
        const String fsCode = LoadShaderFile(fsFile, fsIncludes);
        *ptr = LoadShaderFromMemory(vsCode.c_str(), fsCode.c_str());
        CHECK_RETURN_LOG(!IsShaderValid(*ptr), "Compile failed. fs: " +  fsFile + " vs: " + vsFile, true);
        ptr->locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(*ptr, "mvp");
        ptr->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(*ptr, "instanceTransform");
    }
    
    LoadDefaultLocs();

    return true;
}

bool ShaderResource::Unload()
{
    if (ptr)
        UnloadShader(*ptr);
    delete ptr;
    ptr = nullptr;
    fsIncludes.clear();
    vsIncludes.clear();
    locations.clear();
    return true;
}

Utility::Timepoint ShaderResource::GetEditTime() const
{
    if (defines != Rendering::Manager::Get().GetConfig().Context.Get().GlobalDefines.Get())
        return std::chrono::high_resolution_clock::now();
    
    Utility::Timepoint max = Utility::Timepoint::min(); 
    for (String path : fsIncludes)
        max = Utility::Math::Max(max, Utility::GetFileWriteTime(path));
    for (String path : vsIncludes)
        max = Utility::Math::Max(max, Utility::GetFileWriteTime(path));
    return  max;
}

Shader* ShaderResource::Get() const
{
    if (ptr && IsShaderValid(*ptr))
        return ptr;
    if (identifier.ends_with(".ds"))
    {
        if (const auto defaultShader = ResShader("Shaders/SH_Default.ds").Get())
            if (defaultShader != this)
                return defaultShader->Get();
    }
    else if (identifier.ends_with(".ps"))
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
    PROFILE_GL();
    const Shader* shaderPtr = Get();
    CHECK_RETURN(!shaderPtr, -1);
    const auto find = locations.find(InValue);
    if (find != locations.end())
        return find->second;
    const int loc = rlGetLocationUniform(shaderPtr->id, InValue.c_str());
    locations[InValue] = loc; 
    return loc; 
}

int ShaderResource::GetLocation(const DefaultLoc& InLoc) const
{
    return defaultLocs[static_cast<size_t>(InLoc)];
}

void ShaderResource::LoadDefaultLocs()
{
    PROFILE_GL_GPU("Default unform locations");
    defaultLocs[static_cast<size_t>(DefaultLoc::TIME)] = rlGetLocationUniform(ptr->id, "Time");
    defaultLocs[static_cast<size_t>(DefaultLoc::DELTA)] = rlGetLocationUniform(ptr->id, "Delta");
    defaultLocs[static_cast<size_t>(DefaultLoc::RESOLUTION)] = rlGetLocationUniform(ptr->id, "Resolution");
    defaultLocs[static_cast<size_t>(DefaultLoc::VIEW_PROJ)] = rlGetLocationUniform(ptr->id, "ViewProj");
    defaultLocs[static_cast<size_t>(DefaultLoc::VIEW_PROJ_PREV)] = rlGetLocationUniform(ptr->id, "ViewProjPrev");
    defaultLocs[static_cast<size_t>(DefaultLoc::VIEW_PROJ_INV)] = rlGetLocationUniform(ptr->id, "ViewProjInv");
    defaultLocs[static_cast<size_t>(DefaultLoc::RECT)] = rlGetLocationUniform(ptr->id, "Rect");
    defaultLocs[static_cast<size_t>(DefaultLoc::REF_RECT)] = rlGetLocationUniform(ptr->id, "RefRect");
    defaultLocs[static_cast<size_t>(DefaultLoc::CAMERA_POSITION)] = rlGetLocationUniform(ptr->id, "CameraPosition");
    defaultLocs[static_cast<size_t>(DefaultLoc::NEAR_FAR)] = rlGetLocationUniform(ptr->id, "NearFar");
    defaultLocs[static_cast<size_t>(DefaultLoc::DEFERRED_ID)] = rlGetLocationUniform(ptr->id, "DeferredID");
}

String ShaderResource::LoadShaderFile(const String& InPath, Set<String>& InIncludes)
{
    InIncludes.clear();
    String shader = Utility::ReadFile(InPath);
    shader = ProcessDefines(shader);
    shader = ProcessIncludes(shader, InPath, InIncludes);

    // Fix null terminations
    std::erase(shader, '\0');
    if (shader.find("#version") == std::string::npos)
        shader = "#version 330\n" + shader;
    
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
    defines = c.Context.Get().GlobalDefines;

    // Offset for #version
    const auto verStart = result.find_first_of("#version");
    const auto verEnd = result.find_first_of('\n', verStart);
    for (auto& def : c.Context.Get().GlobalDefines.Get())
        if (!def.empty())
            result.insert(verEnd + 1, "\n#define " + def + "\n");

    return result;
    
    // Find defines
    Set<String> defineFinds;
    std::function foundDefine = [&](const String& InFindStr) -> String
    {
        const auto start = InFindStr.find_first_not_of(' ');
        const auto end = InFindStr.find_last_not_of(' ');
        defineFinds.insert(InFindStr.substr(start, end - start));
        return "";
    };

    // ifdefs
    std::function foundIfdef = [&](const String& InFindStr) -> String
    {
        String str = InFindStr;
        const bool negated = InFindStr.starts_with("ndef");
        if (negated)
            str = str.substr(4);

        const auto defEnd = str.find_first_of('\n');
        const auto defStart = str.find_first_not_of(' ');
        const String def = str.substr(defStart, defEnd - defStart);
        
        if (defineFinds.contains(def) != negated)
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
