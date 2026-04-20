#include "Material.h"

bool Rendering::MaterialResource::Load()
{
    bool load = false;
    if (ShaderResource::Accept(id.Str()))
        data.Shader = ResShader(id);
    else load = PropertyFile::Load();
    
    struct HashData
    {
        uint32 id;
        Utility::Timepoint time;
    } data;
    data.id = id.Hash();
    data.time = GetEditTime();
    hash = Utility::Hash(data);
    
    return load;
}

Utility::Timepoint Rendering::MaterialResource::GetEditTime() const
{
    Utility::Timepoint maxTime = PropertyFile::GetEditTime();
    if (!ShaderResource::Accept(id.Str()))
        if (const ShaderResource* rsc = data.Shader.Get().Get())
            maxTime = Utility::Math::Max(maxTime, rsc->GetEditTime());
    return maxTime;
}

ResShader Rendering::MaterialResource::GetShader() const
{
    return data.Shader;
}

bool Rendering::MaterialResource::Accept(const String &InPath)
{
    return (InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("RM_"))
        || ShaderResource::Accept(InPath); // Also accept shaders
}
