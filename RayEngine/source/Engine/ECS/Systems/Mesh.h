#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/Rendering/Resources/Model.h"
#include "Engine/Rendering/Resources/Material.h"

namespace ECS
{
    struct Mesh : Component
    { 
        PROPERTY_P(bool, Visible, true);
        PROPERTY_P(ResModel, Model, "test.obj");
        PROPERTY_P(ResMaterial, Material, "test_mat.json");
    };

    class SysMesh : public System<Mesh>
    {
        
    public:
        
        void Update(EntityID InEntity, Mesh& InComponent, double InDelta) override;
        bool ShouldUpdate() const override { return true; }
        
    };
}

