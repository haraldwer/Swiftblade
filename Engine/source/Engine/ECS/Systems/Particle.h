#pragma once
#include "ECS/Component.h"
#include "ECS/System.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Particle.h"

namespace ECS
{
    struct Particle : Component<Particle>
    {
        PROPERTY_D(ResParticle, Resource, "Gameplay/Player/PS_Sword.json")
        PROPERTY_D(ResRM, Material, "Defaults/RM_ParticleDefault.json")
        PROPERTY_D(ResModel, Model, "Defaults/M_Cube.obj")
        PROPERTY_D(bool, Shadows, true);
        PROPERTY_D(bool, Lumin, true);
        PROPERTY_D(bool, Visible, true);
        uint32 id;
    };

    class SysParticle : public System<Particle>
    {
    public:
        void Frame(EntityID InID, Particle& InComponent) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -101; }
    };
    
};
