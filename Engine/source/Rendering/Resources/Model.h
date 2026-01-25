#pragma once

#include <webgpu/webgpu.hpp>

#include "Core/Resource/Resource.h"

struct Model;

namespace Rendering
{
    class ModelResource : public Resource::Base
    {
        CLASS_INFO(ModelResource, Resource::Base)
    public:
        bool Load() override;
        bool Unload() override;
        bool Edit(const String &InName, uint32 InOffset = 0) override;
        
        static bool Accept(const String& InPath)
        {
            return InPath.ends_with(".obj");          
        }
        
        void GetMeshState(const wgpu::PrimitiveState& InState) const { }
        
    private:
    };
}

typedef Resource::Ref<Rendering::ModelResource> ResModel;  
