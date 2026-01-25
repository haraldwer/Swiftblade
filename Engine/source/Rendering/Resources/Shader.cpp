#include "Shader.h"
#include "Context/Context.h"

bool Rendering::ShaderResource::Load()
{
    // Create shader
    String file = Utility::File::Read(id.Str());
    CHECK_RETURN_LOG(file.empty(), "Failed to read shader file: " + id.Str(), false)
    
    wgpu::StringView shaderSource = wgpu::StringView(file);
    wgpu::ShaderModuleDescriptor shaderDesc;
    shaderDesc.label = wgpu::StringView("Default shader");
    wgpu::ShaderSourceWGSL shaderCodeSource;
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
        shader.release();
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
        !name.contains("."));
}
