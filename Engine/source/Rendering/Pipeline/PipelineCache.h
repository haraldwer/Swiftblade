#pragma once

#include <webgpu/webgpu.hpp>

#include "Utility/Singelton.h"
#include "Resources/Material.h"

namespace Rendering
{
    class PipelineCache : public Utility::Singelton<PipelineCache, true>
    {
    public:
        void Init();
        void Deinit();
        wgpu::RenderPipeline GetPipeline(const MaterialResource& InMaterial, const Vector<wgpu::ColorTargetState>& InTargetStates, const wgpu::PrimitiveState& InMeshState);
        
    private:
        wgpu::RenderPipeline CreatePipeline(const MaterialResource& InMaterial, const Vector<wgpu::ColorTargetState>& InTargetStates, const wgpu::PrimitiveState& InMeshState);
        
        Map<uint64, wgpu::RenderPipeline> cache;
        // TODO: Also track lifetime!
        // TODO: And load from file!
    };
}
