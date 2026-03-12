#pragma once
#include "webgpu/webgpu.h"

struct Shader;

namespace Rendering
{
    class ShaderResource : public Resource::Base
    {
        friend class PipelineCache;
        
        CLASS_INFO(ShaderResource, Resource::Base)
        
    public:
        bool Load() override;
        bool Unload() override;
        static bool Accept(const String& InPath);
        
    private:
        WGPUShaderModule shader;
    };
}

typedef Resource::Ref<Rendering::ShaderResource> ResShader;
