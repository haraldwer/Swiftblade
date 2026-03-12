#include "Shader.h"
#include "Context/Context.h"

bool Rendering::ShaderResource::Load()
{
    // Create shader
    String file = Utility::File::Read(id.Str());
    CHECK_RETURN_LOG(file.empty(), "Failed to read shader file: " + id.Str(), false)
    
    LOG("Loading shader: " + id.Str());
    WGPUStringView shaderSource = WGPUStringView(file.c_str());
    WGPUShaderModuleDescriptor shaderDesc;
    shaderDesc.label = WGPUStringView("Default shader");
    WGPUShaderSourceWGSL shaderCodeSource;
    shaderCodeSource.chain.next = nullptr;
    shaderCodeSource.chain.sType = WGPUSType_ShaderSourceWGSL;
    shaderDesc.nextInChain = &shaderCodeSource.chain;
    shaderCodeSource.code = shaderSource;
    shader = Context::Get().CreateShader(shaderDesc);
    
    return true;
}

bool Rendering::ShaderResource::Unload()
{
    if (shader)
    {
        wgpuShaderModuleRelease(shader);
        shader = {};
    }
    return true;
}

bool Rendering::ShaderResource::Accept(const String &InPath)
{
    auto name = Utility::File::Name(InPath);
    return name.starts_with("SH_") &&
        (InPath.ends_with(".vs") ||
        InPath.ends_with(".fs") ||
        InPath.ends_with(".ds") ||
        InPath.ends_with(".ps") ||
        InPath.ends_with(".si") ||
        InPath.ends_with(".wgsl") ||
        !name.contains("."));
}
