#pragma once

#include "Utility/Collections/PersistenceContainer.h"
#include "Pipeline/Pipeline.h"
#include "Scene/Instances/ParticleInstance.h"

namespace Rendering
{
    struct ContextConfig;

    struct ParticleInstanceData
    {
        ParticleInstance instance;
        RenderTarget* target = nullptr;
    };
    
    class Particles
    {
    public:
        void Init(const ContextConfig& InConfig);
        void Deinit();

        Pipeline::Stats Update(const RenderArgs& InArgs);
        Vector<ParticleInstanceData> GetFrameInstances(const RenderArgs& InArgs);

    private:
        static bool TryCreate(SwapTarget& InTarget, const ParticleInstance& InValue);

        Utility::PersistenceContainer<uint32> persistence;
        Map<uint32, SwapTarget> particles;

        // TODO: Also reuse textures
        
    };
}

