#pragma once

#include "Pipeline/Renderer.h"
#include "TextureTargets/FrameTargetCollection.h"

namespace Rendering
{
    struct ParticleInstance;

    class ParticlesRenderer : public Renderer
    {
    public:
        static int Simulate(const RenderArgs& InArgs, SwapTarget& InTarget, const ParticleInstance& InInstance);
        static int DrawParticles(const RenderArgs& InArgs, const RenderTarget& InSceneTarget);
    };
}
