#pragma once

#include "RenderingOld/Resources/Material.h"
#include "RenderingOld/Resources/Model.h"
#include "Resources/Particle.h"
#include "Scene/Culling/VisibilityMask.h"

namespace Rendering
{
    struct ParticleInstance
    {
        ResParticle particle;
        ResRM material;
        ResModel model;
        Mat4F transform;
        double startTimestamp;
        float extent = 1.0f;
        uint32 id = 0;
        uint8 mask = static_cast<uint8>(VisibilityMask::ALL);

        static uint64 GenHash(const ResModel& InModel, const ResRM& InMat)
        {
            union Union
            {
                struct Res
                {
                    uint32 materialHash;
                    uint32 modelHash;
                } resource;
                uint64 key = 0;
            } hash;
                
            if (const auto mat = InMat.Get())
                hash.resource.materialHash = mat->Hash();
            if (const auto m = InModel.Get())
                hash.resource.modelHash = m->Hash();

            // TODO: More generic hashing system...
            
            return hash.key;
        }
    };
}
